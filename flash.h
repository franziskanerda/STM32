#ifndef __FLASH_H
#define __FLASH_H 1
/** #INCLUDES          *******************************************************************************/
/** #DEFINES           *******************************************************************************/
// Define the STM32F10x FLASH Page Size depending on the used STM32 device
#ifdef STM32F10X_LD
#define FLASH_PAGE_SIZE    ( (u16)0x400 )
#elif defined STM32F10X_LD_VL
#define FLASH_PAGE_SIZE    ( (u16)0x400 )
#elif defined STM32F10X_MD
#define FLASH_PAGE_SIZE    ( (u16)0x400 )
#elif defined STM32F10X_MD_VL
#define FLASH_PAGE_SIZE    ( (u16)0x400 )
#elif defined STM32F10X_HD
#define FLASH_PAGE_SIZE    ( (u16)0x800 )
#elif defined STM32F10X_HD_VL
#define FLASH_PAGE_SIZE    ( (u16)0x800 )
#elif defined STM32F10X_XL
#define FLASH_PAGE_SIZE    ( (u16)0x800 )
#elif defined STM32F10X_CL
#define FLASH_PAGE_SIZE    ( (u16)0x800 )
#endif // ifdef STM32F10X_LD

/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/

/** Public Functions   *******************************************************************************/
s16 EraseFlash( void *Start, void *End );
s16 WriteFlash( void *Src, void *Dest, s32 ByteCnt );

#endif // FLASH_H