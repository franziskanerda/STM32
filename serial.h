#ifndef __SERIAL_H
#define __SERIAL_H 1

/** #INCLUDES          *******************************************************************************/

/** #DEFINES           *******************************************************************************/
#define TELE_HEADERSIZE (4+2+2+1+3)     // SNR + DCI + TID + TCN + DBS
#define TELE_TAILSIZE   (2)             // CRC
#define TX_DATA_SIZE    256 //1024 * 4	// verkleinert, um SRAM zu sparen
#define RX_DATA_SIZE    256
#define TX_BUF_SIZE     (1 + 2*(TELE_HEADERSIZE + TX_DATA_SIZE + TELE_TAILSIZE) + 1)
#define RX_BUF_SIZE     (1 + 2*(TELE_HEADERSIZE + RX_DATA_SIZE + TELE_TAILSIZE) + 1)

// Zustände der Statemachine des Empfangspuffers
#define RX_BUF_STATUS_READY      0  // Ruhezustand
#define RX_BUF_STATUS_RECEIVE    1  // Telegrammstart empfangen, Telegramm noch nicht vollständig
#define RX_BUF_STATUS_FILLED     3  // Telegrammende empfangen, Puffer bereit zur Auswertung

#define AT_ERR_BADTELE  -1
#define AT_ERR_CRC      -2
#define AT_ERR_NOTME    -3

#define NUMBER_OF_COM_CHANNELS 1
#define COM_CHANNEL_RS    0
#define COM_CHANNEL_LASER 1

#define COM_CHANNEL_DEBUG COM_CHANNEL_RS

#define COM_MODE_TELE     0

#define CHAR_CR     0x0D
#define CHAR_LF     0x0A

/** ENUMS              *******************************************************************************/

/** STRUCTS            *******************************************************************************/
typedef struct RxBufStrct_s
{
	u8  buf[RX_BUF_SIZE]; 	// ACHTUNG: obwohl dies ein byte buffer ist muss er an eine dword Grenze ausgerichtet sein!!!!!!
	u8  *rdptr;           	// Position innerhalb von buf an der momentan ausgewertet wird
	u8  *wrptr;           	// für USB: Position innerhalb von buf an der geschrieben wird
	u8  *last_wrptr;      	// für USB: letzter wrptr damit nur bei neu eingegangenen Bytes ausgewertet wird
	u32 timeout_time;     	// Zeitstempel für Empfangstimeout
	u32 cnt;              	// Anzahl ausgewerteter Bytes ab dem Telegramm-Startzeichen
	u16 tid;               	// Die empfangene Telegram-ID
	u16 dma_cntr;          	// letzter Stand des Hardware-DMA-Zählers damit nur bei neu eingegangenen Bytes ausgewertet wird
	volatile u8 status;    	// siehe #defines RX_BUF_STATUS_xxx
	u8 mode;               	// siehe #defines COM_MODE_xxx
	u8 ESC_received;       	// Flag ob ESC-Character empfangen wurde
	u8 channel;            	// COM_CHANNEL_ zu der diese Struktur gehört, damit antwortende Programmteile
                         	// wissen auf welchem Kanal zu antworten ist.
	u8 tcn;                	// Telegram consecutive number des Anfragetelegrams
	u8 last_tcn;           	// Telegram consecutive number des vorherigen Anfragetelegrams
} RxBufStrct;

/** CONSTS             *******************************************************************************/

/** Public Variables   *******************************************************************************/

/** Public Functions   *******************************************************************************/
s16  InitSerial( u8 Channel, u32 Baudrate, u8 Mode );
void InitSerialLaser( u32 Baudrate );
s16  SendData( const void *Data, u32 DataLen, u8 Channel );
s16  SendTeleFirst( u16 Tid, RxBufStrct *rxb );
s16  SendTeleNext( const void *Data, u32 DataLen );
s16  SendTeleLast( RxBufStrct *rxb );
s16  SendTele( u16 Tid, const void *Data, u32 DataLen, RxBufStrct *rxb );
s16  SendTeleCh( u16 Tid, const void *Data, u32 DataLen, u8 Channel );
s16  SendTeleNack( s16 MajorErr, s16 MinorErr, RxBufStrct *rxb );
s16  SendTeleAck( RxBufStrct *rxb );
void ResetRxBuffer( u8 Channel );
void RxTsk_Wait( void );
void CheckForCommTimeout( void );
void WaitTillSent( void );
s16  CheckForTele( u8 Channel );

#endif // SERIAL_H
