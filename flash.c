/** Description        *******************************************************************************/
/** #INCLUDES          *******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "globals.h"
#include "flash.h"
#include "syslib.h"
#include "serial.h"
#include "info.h"

/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/
/** Public Functions   *******************************************************************************/
/** Private Variables  *******************************************************************************/
/** Private Functions  *******************************************************************************/

/** Code               *******************************************************************************/

/******************************************************************************************************
                                              EraseFlash
-------------------------------------------------------------------------------------------------------
Löscht die Sektoren im Flash die vom angegebenen Adressbereich betroffen sind.

Parameter:
   StartAdr: Anfangsadresse des zu löschenden Bereichs
   EndAdr:   Endadresse des zu löschenden Bereichs

Return:  0 - OK
        <0 - Fehler
******************************************************************************************************/
s16 EraseFlash( void *Start, void *End ) 
{
   volatile FLASH_Status status;
   u8 *StartAdr = Start;
   u8 *EndAdr = End;

   if ( StartAdr > EndAdr ) 
      return -1;

   FLASH_Unlock();
   // Clear All pending flags
   FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );

   status = FLASH_COMPLETE;

   while ( StartAdr <= EndAdr ) 
   {
	  status = FLASH_ErasePage( (u32)StartAdr );
      if ( status != FLASH_COMPLETE ) 
         break;
      StartAdr += FLASH_PAGE_SIZE;
   }

   FLASH_Lock();

   if ( status == FLASH_COMPLETE ) return 0;
   else return -2;
}

/******************************************************************************************************
                                              WriteFlash
-------------------------------------------------------------------------------------------------------
Schreibt count Words von der Adresse source_ptr ins Flash ab der Adresse dest_ptr

Parameter:
   SrcAdr:   Zeiger auf zu schreibende Daten
   DestAdr:  Zieladresse im Flash
   ByteCnt:  Anzahl an zu schreibenden Bytes. Wird wenn nötig auf volle 4-Bytes aufgerundet

Return: 0 = ok
       -1 = Fehler in Parametern
       -2 = Programmieren fehlgeschlagen
       -3 = Verify fehlerhaft
******************************************************************************************************/
s16 WriteFlash( void *Src, void *Dest, s32 ByteCnt ) 
{
	s32 cnt;
	u32 dst;
	u32 *src;
	volatile FLASH_Status status;
	s16 retval = -1;
	u16 retry;

	// Zieladresse muss 4-Byte aligned sein
	if ( ( ( (u32)Dest ) % 4 ) != 0 ) return -1;

	FLASH_Unlock();

	// Clear All pending flags
	FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );

	// Programm
	dst = (u32)Dest;
	src = (u32 *)Src;
	cnt = ByteCnt;

	while ( cnt > 0 )
	{
		retry = 0;
		while ( retry < 5 )
		{
			status = FLASH_ProgramWord( dst, *src );
			retry++;
			if ( status == FLASH_COMPLETE ) break;
		}
		if ( status != FLASH_COMPLETE )
		{
			retval = -2;
			goto end;
		}
		dst += 4;
		src++;
		cnt -= 4;
	}

	// Verify
	dst = (u32)Dest;
	src = (u32 *)Src;
	cnt = ByteCnt;

	while ( cnt > 0 )
	{
		if ( *src != *( (u32 *)dst ) )
		{
			retval = -3;
			goto end;
		}
		dst += 4;
		src++;
		cnt -= 4;
	}

	retval = 0;

end:
  	FLASH_Lock();
  	return retval;
}
