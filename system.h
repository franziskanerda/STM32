#ifndef __SYSTEM_H
#define __SYSTEM_H 1

#include "stm32f10x.h"
#include "core_cm3.h"

#define AND &&
#define OR ||

#define PI 3.141592653589

#define ExitLowPowerState() ( SCB->SCR &= (~NVIC_LP_SLEEPONEXIT) )

typedef enum {
   OFF = 0, ON = !OFF
} OnOffState;

typedef enum {
   NO = 0, YES = !NO
} YesNoState;

typedef unsigned long long u64;
typedef signed long long s64;

#define mem_b( adr )   ( ( (volatile unsigned char *)0 )[adr] )
#define mem_w( adr )   ( ( (volatile unsigned short int *)0 )[(adr)/2] )
#define mem_dw( adr )  ( ( (volatile unsigned long int *)0 )[(adr)/4] )


/* Die Macros SET und RST dienen zum setzen bzw. löschen von einzelnen Bits in Registern, */
/* die sowohl lesbar als auch schreibbar sind (prozessorinterne Register) */
/* Zum gleichzeitigen manipulieren von n Bits dienen die Makros SETn bzw. RSTn*/
#define SET( reg, bit ) ( reg |= ( 1UL << (bit) ) )
#define SET2( reg, bit1, bit2 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) ) )
#define SET3( reg, bit1, bit2, bit3 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) ) )
#define SET4( reg, bit1, bit2, bit3, bit4 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) ) )
#define SET5( reg, bit1, bit2, bit3, bit4, bit5 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) )+ \
                                                           ( 1UL << (bit5) ) )
#define SET6( reg, bit1, bit2, bit3, bit4, bit5, bit6 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) )+ \
                                                                 ( 1UL << (bit5) )+( 1UL << (bit6) ) )
#define SET7( reg, bit1, bit2, bit3, bit4, bit5, bit6, bit7 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+ \
                                                                       ( 1UL << (bit4) )+( 1UL << (bit5) )+( 1UL << (bit6) )+( 1UL << (bit7) ) )
#define SET8( reg, bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8 ) ( reg |= ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+ \
                                                                             ( 1UL << (bit4) )+( 1UL << (bit5) )+( 1UL << (bit6) )+( 1UL << (bit7) )+( 1UL << (bit8) ) )

#define RST( reg, bit ) ( reg &= 0xFFFFFFFF-( 1UL << (bit) ) )
#define RST2( reg, bit1, bit2 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) ) ) )
#define RST3( reg, bit1, bit2, bit3 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) ) ) )
#define RST4( reg, bit1, bit2, bit3, bit4 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) ) ) )
#define RST5( reg, bit1, bit2, bit3, bit4, bit5 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) )+ \
                                                                       ( 1UL << (bit5) ) ) )
#define RST6( reg, bit1, bit2, bit3, bit4, bit5, bit6 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+( 1UL << (bit4) )+ \
                                                                             ( 1UL << (bit5) )+( 1UL << (bit6) ) ) )
#define RST7( reg, bit1, bit2, bit3, bit4, bit5, bit6, bit7 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+ \
                                                                                   ( 1UL << (bit4) )+( 1UL << (bit5) )+( 1UL << (bit6) )+( 1UL << (bit7) ) ) )
#define RST8( reg, bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8 ) ( reg &= 0xFFFFFFFF-( ( 1UL << (bit1) )+( 1UL << (bit2) )+( 1UL << (bit3) )+ \
                                                                                         ( 1UL << (bit4) )+( 1UL << (bit5) )+( 1UL << (bit6) )+( 1UL << (bit7) )+( 1UL << (bit8) ) ) )

#define BIT0   0x0001
#define BIT1   0x0002
#define BIT2   0x0004
#define BIT3   0x0008
#define BIT4   0x0010
#define BIT5   0x0020
#define BIT6   0x0040
#define BIT7   0x0080
#define BIT8   0x0100
#define BIT9   0x0200
#define BIT10  0x0400
#define BIT11  0x0800
#define BIT12  0x1000
#define BIT13  0x2000
#define BIT14  0x4000
#define BIT15  0x8000
#define BIT( n ) ( 1UL << (n) )

// Pseudofunktionen
#define BitIsClr( reg, bit )  ( ( (reg) & ( 1UL << (bit) ) ) == 0 )
#define BitIsSet( reg, bit )  ( ( (reg) & ( 1UL << (bit) ) ) != 0 )

#define ToggleBitNr( reg, bit )      ( (reg) ^= ( 1UL << (bit) ) )
#define ToggleBitMask( reg, mask )   ( (reg) ^= (mask) )

#define ClearBitMask( reg, mask )    ( (reg) &= (~mask) )
#define SetBitMask( reg, mask )      ( (reg) |= (mask) )

#define max( a, b )  ( (a) > (b) ? (a) : (b) )
#define min( a, b )  ( (a) < (b) ? (a) : (b) )

#define VT100_EraseDisplayAll    "\x1B[2J"
#define VT100_CursorUp( x )      "\x1B[" # x "A"

#define USART_BRR_RESET          0x00005A2A
#define USART_BRR_RESETBOOT      0x00005A2B
#define USART_BRR_RESETUSB       0x00005A2C
#define USART_BRR_RESETBOOTUSB   0x00005A2D
#endif // ifndef __SYSTEM_H
