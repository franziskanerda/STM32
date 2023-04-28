#ifndef __CRC_H
#define __CRC_H 1

/**********   #INCLUDES            **********/
/**********   #DEFINES             **********/
/**********   ENUMS                **********/
/**********   STRUCTS              **********/
/**********   CONSTS               **********/
/**********   Public Variables     **********/

/**********   Public Functions     **********/
u16 CalcCrc16( register u16 Crc, register const void *Start, register u32 Len );
u16 CalcCrc16RunTasks( register u16 Crc, register const void *Start, register u32 Len );
u8  CalcCrc8( register u8 crc, void *start, register u32 len );
s16 CheckBootCrc( void );

#endif // ifndef __CRC_H
