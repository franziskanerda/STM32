//#pragma GCC optimize 0

/** Description        *******************************************************************************/
// USART1 wird für RS232 verwendet.

/*
Senden: Es muss zuerst die tx_buf-Struktur initialisiert werden, d.h. zu sendende Daten in
den Sendepuffer kopieren, die Anzahl zu sendender Bytes setzen und den Status auf besetzt stellen.
Dies geschieht in den Funktionen SendData (rohe Daten senden) oder SendTele (Telegramm senden).
Alle diese Funktionen rufen dann SendTxBuffer auf. SendTxBuffer initialisiert die DMA-Engine der
CPU so, dass alle Daten im Puffer ohne weitere Softwarebeteiligung abgesendet werden. Wenn alle
Daten abgesendet wurden generiert die DMA-Engine je nach Kanal die IRQs DMA1_Channel4_IRQHandler/
DMA1_Channel7_IRQHandler. In diesen IRQs wird die DMA-Engine abgeschaltet und der Status des tx_buf
auf idle gestellt.

Empfangen (nur RS232): Die DMA-Engine wird so konfiguriert (in ResetRxBuffer) das empfangene Bytes ohne
Softwarebeteiligung in die jeweiligen Empfangsbuffer (rx_buf[x]->buf) geschrieben werden. Dazu wird
die zu übertragende Länge in der DMA-Engine auf die Grösse de Empfangspuffers gesetzt. Eine Ende-
Erkennung erfolgt durch DMA nicht weil im vorraus ja unbekannt ist wie lange die zu empfangenden
Daten sind. Für die Ende-Erkennung werden die UARTs so konfiguriert das bei einer Idle-Bedingung
(=RX-Leitung ist nach einem empfangegem Zeichen 1.5 x Zeit für ein Byte ruhig) oder bei einem Fehler im
empfangenem Zeichen ein IRQ generiert wird (USART1_IRQHandlerUSART4_IRQHandler).
Der Irq stellt den Empfangstask dann auf "RxTsk_CheckForTele". Der Empfangstask prüft dann den
Empfangsbuffer auf ein vollständig angekommenes Telegramm (CheckForTele) und veranlasst
die Aktion (DoTele).
*/

/** #INCLUDES          *******************************************************************************/
#include <string.h>
#include "system.h"
#include "syslib.h"
#include "globals.h"
#include "serial.h"
#include "crc.h"
#include "teles.h"
#include "task.h"
#include "comm.h"
#include "info.h"

/** #DEFINES           *******************************************************************************/
#define TXBUF_IDLE  0
#define TXBUF_INUSE 1

/** ENUMS              *******************************************************************************/

/** STRUCTS            *******************************************************************************/

// Struktur mit allen Infos zum Senden von Daten
typedef struct TxBufStrct_s
{
	u8  buf[TX_BUF_SIZE];    	// die zu sendenden Daten
	u32 datalen;               	// Anzahl zu sendender Bytes
	u32 datapos;               	// Positionsspeicher. Anzahl der bereits übertragenen Daten von buf
	u16 crc;             		// CRC Pruefsumme
	u8  status;             	// siehe #define TXBUF_
} TxBufStrct;

typedef struct CommChannelStrct_s
{
	RxBufStrct          *rxb;
	USART_TypeDef       *usart;
	DMA_Channel_TypeDef *dma_rx;
	DMA_Channel_TypeDef *dma_tx;
} CommChannelStrct;

typedef struct HeadStrct_s
{
	u32 snr;
	u16 dci;
	u16 tid;
	u8  tcn;
	u8  reserved[3];
} HeadStrct;

/** Private Variables  *******************************************************************************/

// Jeweils einen RX- und TX-Buffer fuer RS232 definieren
static RxBufStrct rx_buf;
static TxBufStrct tx_buf;
static u8 IdleOccured = 0;

/** CONSTS             *******************************************************************************/
const CommChannelStrct CommChannel[] =
{
	 { &rx_buf, USART1, DMA1_Channel5, DMA1_Channel4 },   // RS232 als einziger Kanal
};


/** Private Functions  *******************************************************************************/
s16  WaitForTxBufferFree( void );
s16  SendTxBuffer( u8 Channel );
void RxTsk_CheckForTele_RS( void );
void ResendLastTele( RxBufStrct *rxb );

/** Code               *******************************************************************************/

/******************************************************************************************************
                                              InitSerial
-------------------------------------------------------------------------------------------------------
Initialisiert den angegebenen Kommunikationskanal

Parameter:
   Channel:  siehe #defines COM_CHANNEL_x
   Baudrate: direkter Wert z.B. 115200
   Mode:     siehe #defines COM_MODE_x

Return:  0 = ok
        -1 = unbekannter Channel
******************************************************************************************************/
s16 InitSerial( u8 Channel, u32 Baudrate, u8 Mode )
{
   const CommChannelStrct *cc = &CommChannel[Channel];
   USART_InitTypeDef uis;

   if( Channel >= NUMBER_OF_COM_CHANNELS ) return -1;

   // USART Konfiguration
   if ( cc->usart != NULL )
   {
      uis.USART_BaudRate = Baudrate;
      uis.USART_WordLength = USART_WordLength_8b;
      uis.USART_StopBits = USART_StopBits_1;
      uis.USART_Parity = USART_Parity_No;
      uis.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      uis.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_Init( cc->usart, &uis );

      USART_ITConfig( cc->usart, USART_IT_ERR, ENABLE );
      USART_ITConfig( cc->usart, USART_IT_IDLE, ENABLE );
      USART_DMACmd( cc->usart, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE );
      USART_Cmd( cc->usart, ENABLE );
      DMA_DeInit( cc->dma_rx );
   }

   tx_buf.status = TXBUF_IDLE;
   tx_buf.datalen = 0;
   cc->rxb->mode = Mode;
   cc->rxb->last_tcn = 127;
   ResetRxBuffer( Channel );
   return 0;
}

/******************************************************************************************************
                                          WaitForTxBufferFree
-------------------------------------------------------------------------------------------------------
Wartet max 10ms bis der TX Puffer frei wird. Wenn er dann immer noch belegt ist wird er freigemacht
weil von einem Fehler ausgegangen werden muss.

Return:  0 - TX verfügbar
******************************************************************************************************/
s16 WaitForTxBufferFree( void )
{
   u32 wait;
   u16 tasknr;

   // Ist der Sendepuffer frei?
   TimeoutStart( wait );
   tasknr = SuspendTask();

   while( tx_buf.status != TXBUF_IDLE )
   {
      TaskList();
      if( Timeout( wait, 10 ) ) tx_buf.status = TXBUF_IDLE;
   }

   ResumeTask( tasknr );
   return 0;
}

/******************************************************************************************************
                                             WaitTillSent
-------------------------------------------------------------------------------------------------------
Wartet max 500ms bis das anstehende Telegramm abgesendet ist.

******************************************************************************************************/
void WaitTillSent( void )
{
   u32 wait;

   // Ist der Sendepuffer frei?
   TimeoutStart( wait );

   while( tx_buf.status != TXBUF_IDLE )
   {
      if( Timeout( wait, 500 ) ) return ;
      DelayMs( 1 );
   }

   return ;
}

/******************************************************************************************************
                                             SendTxBuffer
-------------------------------------------------------------------------------------------------------
Schickt den tx_buf über den angegebenen Kommunikationskanal

Parameter:
   Channel:  siehe #defines COM_CHANNEL_x

Return: 0  - ok
******************************************************************************************************/
s16 SendTxBuffer( u8 Channel )
{
   DMA_InitTypeDef dis;
   const CommChannelStrct *cc = &CommChannel[Channel];

   tx_buf.datapos = 0;
   
   // DMA und passenden IRQ für den DMA-Abschluss konfigurieren
   dis.DMA_MemoryBaseAddr = (u32)tx_buf.buf;
   dis.DMA_DIR = DMA_DIR_PeripheralDST;
   dis.DMA_BufferSize = tx_buf.datalen;
   dis.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   dis.DMA_MemoryInc = DMA_MemoryInc_Enable;
   dis.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   dis.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   dis.DMA_Mode = DMA_Mode_Normal;
   dis.DMA_Priority = DMA_Priority_Medium;
   dis.DMA_M2M = DMA_M2M_Disable;

   DMA_DeInit( cc->dma_tx );
   dis.DMA_PeripheralBaseAddr = (u32) &(cc->usart->DR);
   DMA_Init( cc->dma_tx, &dis );
   DMA_ITConfig( cc->dma_tx, DMA_IT_TC, ENABLE );
   DMA_Cmd( cc->dma_tx, ENABLE );

   return 0;
}

/******************************************************************************************************
                                               SendData
-------------------------------------------------------------------------------------------------------
Schickt Daten raw über den Kommunikationskanal

Parameter:
   Data:     Zeiger auf Daten.
   DataLen:  Anzahl zu sendender Bytes
   Channel:  siehe #defines COM_CHANNEL_x

Return:  0 - ok
        -1 - Timeout
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendData( const void *Data, u32 DataLen, u8 Channel )
{
   if( DataLen > TX_BUF_SIZE ) return -2;

   // Ist der Sendepuffer frei?
   if( 0 != WaitForTxBufferFree() ) return -1;

   tx_buf.status = TXBUF_INUSE;
   tx_buf.datalen = DataLen;
   memcpy( tx_buf.buf, Data, DataLen );

   return SendTxBuffer( Channel );
}

/******************************************************************************************************
                                               SendTeleCh
-------------------------------------------------------------------------------------------------------
Sendet ein Telegramm über den angegebenen Kommunikationskanal

Parameter:
  Tid:      Telegramm-ID
  Data:     Zeiger auf Daten
  DataLen:  Anzahl der Daten-Bytes
  Channel:  siehe #defines COM_CHANNEL_x

Return:  0 - ok
        -1 - Timeout
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendTeleCh( u16 Tid, const void *Data, u32 DataLen, u8 Channel )
{
   CommChannel[Channel].rxb->channel = Channel;
   CommChannel[Channel].rxb->tcn = 0;
   return SendTele( Tid, Data, DataLen, CommChannel[Channel].rxb );
}

/******************************************************************************************************
                                              EncodeData
-------------------------------------------------------------------------------------------------------
Kodiert daten nach der NGS Telegrammspezifikation

Parameter:
   Data:     Zeiger auf zu kodierende daten
   DataLen:  Länge der zu kodierenden Daten in byte
   DestPtr:  Zeiger auf Zeiger wohin die kodierten Daten geschrieben werden sollen. Der Zeiger wird
             entsprechend der kodierten Datenmenge weitergeführt.

Return:  0 - ok
        -1 - TX Buffer voll
******************************************************************************************************/
s16 EncodeData( const void *Data, u32 DataLen, u8 **DestPtr )
{
   u8 *txbufend = tx_buf.buf + TX_BUF_SIZE;
   const u8 *sp; // source pointer
   u8  actdata;
   u16 tasknr;
   s16 retval = -1;

   tasknr = SuspendTask();

   // Jetzt die Daten codieren
   sp = (u8 *)Data;
   while ( DataLen > 0 )
   {
      TaskList();
      if( *DestPtr >= txbufend ) {
         goto End;
      }
      actdata = *sp++;

      if ( (actdata >= RS_START_CHAR) AND (actdata <= RS_STOP_CHAR) )
      {
         *(*DestPtr)++ = RS_ESC_CHAR;
         if( *DestPtr >= txbufend ) goto End;
         *(*DestPtr)++ = (u8)(actdata + RS_ESC_OFFSET);
      }
      else
      { *(*DestPtr)++ = actdata; }

      DataLen--;
   }
   retval = 0;

End:
   ResumeTask( tasknr );
   return retval;
}

/******************************************************************************************************
                                             SendTeleFirst
-------------------------------------------------------------------------------------------------------
Erster Teil einer Telegrammübertragung. Schreibt das Startzeichen und den Header in tx_buf.

Parameter:
  Tid:      Telegramm-ID
  rxb:      Zeiger auf Empfangspuffer-Struktur auf deren Request geantwortet wird

Return:  0 - ok
        -1 - Timeout
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendTeleFirst( u16 Tid, RxBufStrct *rxb )
{
   s16 status;
   u8  *dp;      // destination pointer
   HeadStrct head;

   // Ist der Sendepuffer frei?
   if( 0 != WaitForTxBufferFree() ) return -1;

   tx_buf.status = TXBUF_INUSE;

   // TID_SETCHARGING geht an den Laser/Sensor; daher nicht die ID und SN des Rotalign touch verwenden!
   if (Tid == TID_SETCHARGING)
   {
	   head.snr = 0;
	   head.dci = 0;
   }
   // uebrige Protokolle gehen an den iMX6; dafuer die interne ID und SN verwenden!
   else
   {
	   head.snr = SERIALNUMBER;
	   head.dci = DEVICE_CLASS_ID;
   }

   head.tid = Tid;
   head.tcn = rxb->tcn;
   head.reserved[0] = 0;
   head.reserved[1] = 0;
   head.reserved[2] = 0;

   // CRC über den Header rechnen
   tx_buf.crc = CalcCrc16RunTasks( 0, &head, sizeof(head) );

   dp = tx_buf.buf;
   *dp++ = RS_START_CHAR;

   // Jetzt den Header codieren
   status = EncodeData( &head, sizeof(head), &dp );
   if ( status < 0 )
   {
      tx_buf.status = TXBUF_IDLE;
      return -2;
   }
   tx_buf.datalen = (u32)(dp - tx_buf.buf); // Telegrammlänge ausrechnen

   return 0;
}

/******************************************************************************************************
                                               SendTeleNext
-------------------------------------------------------------------------------------------------------
Nächster Teil einer Telegrammübertragung. Schreibt einen weiteren Datenblock nach tx_buf.


Parameter:
  Data:     Zeiger auf Daten
  DataLen:  Anzahl der Daten-Bytes

Return:  0 - ok
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendTeleNext( const void *Data, u32 DataLen )
{
   s16 status;
   u8  *dp;      // destination pointer

   // CRC über die Daten weiterrechnen
   tx_buf.crc = CalcCrc16RunTasks( tx_buf.crc, Data, DataLen );

   dp = tx_buf.buf + tx_buf.datalen;

   // Jetzt die Daten codieren
   status = EncodeData( Data, DataLen, &dp );
   if ( status < 0 )
   {
      tx_buf.status = TXBUF_IDLE;
      return -2;
   }

   tx_buf.datalen = (u32)(dp - tx_buf.buf); // Telegrammlänge ausrechnen
   return 0;
}

/******************************************************************************************************
                                               SendTeleLast
-------------------------------------------------------------------------------------------------------
Letzter Teil einer Telegrammübertragung. Schreibt die CRC und das Stopzeichen und sendet ab.


Parameter:
  rxb:      Zeiger auf Empfangspuffer-Struktur auf deren Request geantwortet wird

Return:  0 - ok
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendTeleLast( RxBufStrct *rxb )
{
   s16 status;
   u8  *dp;      // destination pointer

   dp = tx_buf.buf + tx_buf.datalen;

   // CRC codieren
   status = EncodeData( &tx_buf.crc, sizeof(tx_buf.crc), &dp );

   if ( status < 0 )
   {
      tx_buf.status = TXBUF_IDLE;
      return -2;
   }

   // Mit Telegramstop abschliessen
   *dp++ = RS_STOP_CHAR;

   tx_buf.datalen = (u32)(dp - tx_buf.buf); // Telegrammlänge ausrechnen

   return SendTxBuffer( rxb->channel );
}

/******************************************************************************************************
                                               SendTele
-------------------------------------------------------------------------------------------------------
Sendet ein Telegramm über den angegebenen Kommunikationskanal


Parameter:
  Tid:      Telegramm-ID
  Data:     Zeiger auf Daten
  DataLen:  Anzahl der Daten-Bytes
  rxb:      Zeiger auf Empfangspuffer-Struktur auf deren Request geantwortet wird

Return:  0 - ok
        -1 - Timeout
        -2 - Daten passen nicht in den Sendepuffer
******************************************************************************************************/
s16 SendTele( u16 Tid, const void *Data, u32 DataLen, RxBufStrct *rxb ) {
   s16 status;

   status = SendTeleFirst( Tid, rxb );
   if ( status != 0 ) return status;

   status = SendTeleNext( Data, DataLen );
   if ( status != 0 ) return status;

   return SendTeleLast( rxb );
}

/******************************************************************************************************
                                              SendTeleAck
-------------------------------------------------------------------------------------------------------
Sendet ein Acknowledge.

Parameter:
   Channel:  siehe #defines COM_CHANNEL_x

Return: siehe SendTele
******************************************************************************************************/
s16 SendTeleAck( RxBufStrct *rxb )
{
   u16 status;

   status = sys_status & 0x0000FFFF;
   return SendTele( TID_ACK, &status, sizeof(status), rxb );
}

/******************************************************************************************************
                                             SendTeleNack
-------------------------------------------------------------------------------------------------------
Sendet auf dem angegebenen Kanal ein Nack mit dem angegebenen Fehlercode und Usernumber

Parameter:
   MajorErr: Errornumber
   MinorErr: Unternummer
   Channel:  siehe #defines COM_CHANNEL_x

Return: siehe SendTele
******************************************************************************************************/
s16 SendTeleNack( s16 MajorErr, s16 MinorErr, RxBufStrct *rxb )
{
#pragma pack(2)
   struct
   {
      s16 MajorErr;
      s16 MinorErr;
      u16 status;
   } err;
#pragma pack()

   err.MajorErr = MajorErr;
   err.MinorErr = MinorErr;
   err.status = sys_status & 0x0000FFFF;
   return SendTele( TID_NACK, &err, sizeof(err), rxb );
}

/******************************************************************************************************
                                       DMA1_Channel4_IRQHandler
-------------------------------------------------------------------------------------------------------
Dieser IRQ wird aufgerufen wenn der tx_buf per DMA komplett abgesendet wurde (COM_CHANNEL_RS)
******************************************************************************************************/
void DMA1_Channel4_IRQHandler( void )
{
   // Test on DMA1 Channel7 Transfer Complete interrupt
   if( DMA_GetITStatus( DMA1_IT_TC4 ) )
   {
      USART_ClearFlag( USART1, USART_FLAG_TC ); // Transmission completed Flag löschen
      tx_buf.status = TXBUF_IDLE;
      // Clear DMA1 Channel4 Half Transfer, Transfer Complete and Global interrupt pending bits
      DMA_ClearITPendingBit( DMA1_IT_GL4 );
      DMA_ITConfig( DMA1_Channel4, DMA_IT_TC, DISABLE );
      DMA_Cmd( DMA1_Channel4, DISABLE );
   }
}

/******************************************************************************************************
                                             ResetRxBuffer
-------------------------------------------------------------------------------------------------------
Setzt die rx_buf Struktur zurück

Parameter:
   Channel:  siehe #defines COM_CHANNEL_x
******************************************************************************************************/
void ResetRxBuffer( u8 Channel )
{
   const CommChannelStrct *cc = &CommChannel[Channel];
   DMA_InitTypeDef dis;

   cc->rxb->timeout_time = 0;
   cc->rxb->status = RX_BUF_STATUS_READY;
   cc->rxb->cnt = 0;
   cc->rxb->dma_cntr = sizeof(cc->rxb->buf);
   cc->rxb->rdptr = cc->rxb->buf;
   cc->rxb->wrptr = cc->rxb->buf;
   cc->rxb->last_wrptr = cc->rxb->wrptr;

   if ( cc->dma_rx != NULL )
   {
      dis.DMA_MemoryBaseAddr = (u32)cc->rxb->buf;
      dis.DMA_DIR = DMA_DIR_PeripheralSRC;
      dis.DMA_BufferSize = sizeof(cc->rxb->buf);
      dis.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
      dis.DMA_MemoryInc = DMA_MemoryInc_Enable;
      dis.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
      dis.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
      dis.DMA_Mode = DMA_Mode_Normal;
      dis.DMA_Priority = DMA_Priority_Medium;
      dis.DMA_M2M = DMA_M2M_Disable;
      dis.DMA_PeripheralBaseAddr = (u32) &(cc->usart->DR);

      // DMA initialisieren und einschalten
      DMA_Cmd( cc->dma_rx, DISABLE );
      DMA_Init( cc->dma_rx, &dis );
      DMA_Cmd( cc->dma_rx, ENABLE );
   }
}

/******************************************************************************************************
                                           USART1_IRQHandler
-------------------------------------------------------------------------------------------------------
Dieser IRQ wird aufgerufen wenn ein Idle auf der Leitung erkannt wurde oder beim Empfang Fehler
aufgetreten sind (COM_CHANNEL_RS)
******************************************************************************************************/
void USART1_IRQHandler( void )
{
   u16 dummy;
   u32 sr;

   sr = USART1->SR;
   dummy = USART1->DR; // Read SR register and then DR to clear SR

   if( (sr & 0x0000000F) != 0 )           // Error IRQ
   { ResetRxBuffer( COM_CHANNEL_RS ); }
   else if( (sr & 0x00000010) != 0 )      // Idle IRQ
   {
      IdleOccured = 1;
      if ( IsTaskStopped( RX_TASK ) ) SetTaskState( RX_TASK, RxTsk_CheckForTele_RS );
      ExitLowPowerState();
   }
}

/******************************************************************************************************
                                          RxTsk_CheckForTele
-------------------------------------------------------------------------------------------------------
Wird von den Empfangs-IRQs aktiviert wenn die RX-Leitung einen Idle-Zustand erkennt. Dann wird geprüft
ob ein Telegramm vollständig angekommen ist und wenn ja darauf geantwortet
******************************************************************************************************/
void RxTsk_CheckForTele( u8 Channel )
{
   s16 status;

   status = CheckForTele( Channel );
   if( status == 0 ) DoTele( CommChannel[Channel].rxb );

   StopTask( RX_TASK );
}

void RxTsk_CheckForTele_RS( void )
{
   RxTsk_CheckForTele( COM_CHANNEL_RS );
}

/******************************************************************************************************
                                             CheckForTele
-------------------------------------------------------------------------------------------------------
Prüft auf dem angegebenen Kanal ob ein Telegramm vollständig angekommen ist.
Wenn eine Telegramm angekommen ist stehen in rxb->rdptr die empfangenen Daten, rxb->cnt enthält die
Anzahl der empfangenen Daten in Byte. In rxb->tid steht die empfangene Telegrammnummer, rxb->channel
enthält den Kanal auf dem empfangen wurde.

Parameter:
   Channel:  siehe #defines COM_CHANNEL_x

Return: 0 - vollständiges Telegramm angekommen.
       -1 - kein Telegramm vorhanden
******************************************************************************************************/
s16 CheckForTele( u8 Channel )
{
   u8  data;
   s16 retval = -1;
   u16 tasknr;
   u32 rx_len;
   HeadStrct *head;
   const CommChannelStrct *cc = &CommChannel[Channel];
   RxBufStrct *rxb = cc->rxb;

   if ( IdleOccured == 0 ) return -1;
   IdleOccured = 0;
   tasknr = SuspendTask();

   // Ausrechnen wieviele neue Bytes emfangen wurden.
   if ( cc->dma_rx != NULL )
   {
      // Ausrechnen wieviele neue Bytes emfangen wurden. dma_rx->CNDTR ist der rückwärtszählende Counter
      // der DMA-Hardware (noch zu übertragende Bytes, wird in ResetRxBuffer auf sizeof(rxb->buf)
      // initialisiert)
      rx_len = rxb->dma_cntr - cc->dma_rx->CNDTR;
      rxb->dma_cntr = cc->dma_rx->CNDTR;                 // Zählerstand der Hardware merken fürs nächste mal
   }
   else
   {
      rx_len = rxb->wrptr - rxb->last_wrptr;
      rxb->last_wrptr = rxb->wrptr;
   }

   switch( rxb->mode )
   {
      case COM_MODE_TELE:
         while ( rx_len-- > 0 )
         {
            data = *rxb->rdptr++;

            if( data == RS_START_CHAR )
            {
               rxb->status = RX_BUF_STATUS_RECEIVE;   // wechseln in Empfangszustand
               rxb->cnt = 0;
               rxb->ESC_received = 0;
               TimeoutStart( rxb->timeout_time );     // Start Timeout für den vollständigen Empfang
            }
            else if( data == RS_ESC_CHAR )
            {
               rxb->ESC_received = 1;
            }
            else if( data == RS_STOP_CHAR )
            {
               u16 crc_ist, crc_soll;

               // Es müssen wenigstens ein Header und eine CRC empfangen worden sein
               if ( rxb->cnt < ( sizeof(HeadStrct) + sizeof(crc_soll) ) )
               {
                  ResetRxBuffer( Channel );
                  goto End;
               }

               head = (HeadStrct *)rxb->buf;

               // Seriennummer und DeviceClass prüfen. Wert 0 oder richtiger Wert wird jeweils akzeptiert.
               if ( ( ( head->snr != 0 ) AND ( head->snr != SERIALNUMBER ) )
            	 OR ( ( head->dci != 0 ) AND ( head->dci != DEVICE_CLASS_ID ) ) )
               {
                  ResetRxBuffer( Channel );
                  goto End;
               }

               rxb->cnt -= 2; // die beiden Bytes von der CRC abziehen

               // CRC prüfen
               crc_ist = CalcCrc16( 0, rxb->buf, rxb->cnt );
               rxb->rdptr = rxb->buf + rxb->cnt + 1;     // rdptr zeigt auf das hintere Byte der CRC
               crc_soll = *rxb->rdptr--;
               crc_soll <<= 8;
               crc_soll |= *rxb->rdptr;

               // Channel und tcn werden im Fehlerfall fürs Nack gebraucht deshalb schon hier speichern
               rxb->channel = Channel;
               rxb->tcn = head->tcn;

               if( (crc_ist != crc_soll) AND (crc_soll != 0) )
               {
                   SendTeleNack( TERR_BAD_CRC, 0, rxb );
                   ResetRxBuffer( Channel );
                   goto End;
               }

               // DMA abschalten
               if ( cc->dma_rx != NULL )
               {
                   DMA_Cmd( cc->dma_rx, DISABLE );
               }

               // Zur Sicherheit den Datenbereich mit einer 0 terminieren. Falls jemand mit Strings
               // im Datenbereich arbeitet sind die dann zuverlässig abgeschlossen.
               *rxb->rdptr = 0;

               // Restlichen RX-Buffer aufbereiten
               rxb->status = RX_BUF_STATUS_FILLED;
               rxb->tid = head->tid;
               rxb->rdptr = rxb->buf + sizeof(HeadStrct);
               rxb->cnt -= sizeof(HeadStrct);

               // Prüfen ob das angekommene Telegram eine Aufforderung zum Resend des letzten
               // Telegramms ist. Dies wird hier zentral abgehandelt.
               //if ( (rxb->tid == TID_RESEND_LAST_TELE) OR (rxb->tcn == rxb->last_tcn) )
               //{
               //   ResendLastTele( rxb );
               //}
               //else
               {
                  rxb->last_tcn = rxb->tcn;
                  retval = 0;
                  TimeoutStart( LastCommTime );
               }
               goto End;
            }
            else
            {
               if( rxb->ESC_received == 1 )
               {
                  rxb->ESC_received = 0;
                  data -= RS_ESC_OFFSET;
               }
               rxb->buf[rxb->cnt++] = data;
            }
         }
      break;
   }

End:
	ResumeTask( tasknr );
	return retval;
}

/******************************************************************************************************
                                            CheckForCommTimeout
-------------------------------------------------------------------------------------------------------
Wird vom Sage-Task alle 10ms aufgerufen. Prüft ob einer der Kanäle mit einem unfertigen Telegramm
ins Timeout läuft und resettet den Kanal wenn nötig.
******************************************************************************************************/
void CheckForCommTimeout( void ) {
   u16 n;
   RxBufStrct *rxb;

   for ( n = 0 ; n < NUMBER_OF_COM_CHANNELS ; n++ )
   {
      rxb = CommChannel[n].rxb;
      // Prüfen ob ein Timeout während des Empfangs aufgetreten ist
      if( rxb->timeout_time != 0 )
      {
         if( Timeout( rxb->timeout_time, 10000 ) ) {
            ResetRxBuffer( n );
         }
      }
   }
}

/******************************************************************************************************
                                            ResendLastTele
-------------------------------------------------------------------------------------------------------


Parameter:
   rxb:
******************************************************************************************************/
void ResendLastTele( RxBufStrct *rxb )
{
   tx_buf.status = TXBUF_INUSE;
   SendTxBuffer( rxb->channel );
   ResetRxBuffer( rxb->channel );
}

//#pragma GCC reset_options
