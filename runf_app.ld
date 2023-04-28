EXTERN( __io_putchar )
EXTERN( _write )

/* default stack sizes.
   These are used by the startup in order to allocate stacks for the different modes.
*/
__Stack_Size = 3072 ; 
/* __Stack_Size = 2048 ; */

PROVIDE ( _Stack_Size = __Stack_Size ) ;

__Stack_Init = _estack  - __Stack_Size ;

/*"PROVIDE" allows to easily override these values from an object file or the commmand line.*/
PROVIDE ( _Stack_Init = __Stack_Init ) ;

/* There will be a link error if there is not this amount of RAM free at the end. */
_Minimum_Stack_Size = 3072 ; 
/* _Minimum_Stack_Size = 2048 ; */ 



/* this sends all unreferenced IRQHandlers to the Default_Handler */
PROVIDE (   Reset_Handler = Default_Handler ) ;
PROVIDE (   SWI_Handler = Default_Handler ) ;
PROVIDE (   IRQ_Handler = Default_Handler ) ;
PROVIDE (   Prefetch_Handler = Default_Handler ) ;
PROVIDE (   Abort_Handler = Default_Handler ) ;
PROVIDE (   FIQ_Handler = Default_Handler ) ;
PROVIDE (   NMI_Handler = Default_Handler ) ;
PROVIDE (   HardFault_Handler = Default_Handler ) ;
PROVIDE (   MemManage_Handler = Default_Handler ) ;
PROVIDE (   BusFault_Handler = Default_Handler ) ;
PROVIDE (   UsageFault_Handler = Default_Handler ) ;
PROVIDE (   SVC_Handler = Default_Handler ) ;
PROVIDE (   DebugMon_Handler = Default_Handler ) ;
PROVIDE (   PendSV_Handler = Default_Handler ) ;
PROVIDE (   SysTick_Handler = Default_Handler ) ;
PROVIDE (   WWDG_IRQHandler = Default_Handler ) ;
PROVIDE (   PVD_IRQHandler = Default_Handler ) ;
PROVIDE (   TAMPER_IRQHandler = Default_Handler ) ;
PROVIDE (   RTC_IRQHandler = Default_Handler ) ;
PROVIDE (   FLASH_IRQHandler = Default_Handler ) ;
PROVIDE (   RCC_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI0_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI1_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI2_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI3_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI4_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel1_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel2_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel3_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel4_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel5_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel6_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA1_Channel7_IRQHandler = Default_Handler ) ;
PROVIDE (   ADC1_2_IRQHandler = Default_Handler ) ;
PROVIDE (   USB_HP_CAN1_TX_IRQHandler = Default_Handler ) ;
PROVIDE (   USB_LP_CAN1_RX0_IRQHandler = Default_Handler ) ;
PROVIDE (   CAN1_RX1_IRQHandler = Default_Handler ) ;
PROVIDE (   CAN1_SCE_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI9_5_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM1_BRK_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM1_UP_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM1_TRG_COM_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM1_CC_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM2_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM3_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM4_IRQHandler = Default_Handler ) ;
PROVIDE (   I2C1_EV_IRQHandler = Default_Handler ) ;
PROVIDE (   I2C1_ER_IRQHandler = Default_Handler ) ;
PROVIDE (   I2C2_EV_IRQHandler = Default_Handler ) ;
PROVIDE (   I2C2_ER_IRQHandler = Default_Handler ) ;
PROVIDE (   SPI1_IRQHandler = Default_Handler ) ;
PROVIDE (   SPI2_IRQHandler = Default_Handler ) ;
PROVIDE (   USART1_IRQHandler = Default_Handler ) ;
PROVIDE (   USART2_IRQHandler = Default_Handler ) ;
PROVIDE (   USART3_IRQHandler = Default_Handler ) ;
PROVIDE (   EXTI15_10_IRQHandler = Default_Handler ) ;
PROVIDE (   RTCAlarm_IRQHandler = Default_Handler ) ;
PROVIDE (   USBWakeUp_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM8_BRK_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM8_UP_IRQHandler = Default_Handler );
PROVIDE (   TIM8_TRG_COM_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM8_CC_IRQHandler = Default_Handler ) ;
PROVIDE (   ADC3_IRQHandler = Default_Handler ) ;
PROVIDE (   FSMC_IRQHandler = Default_Handler ) ;
PROVIDE (   SDIO_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM5_IRQHandler = Default_Handler ) ;
PROVIDE (   SPI3_IRQHandler = Default_Handler ) ;
PROVIDE (   UART4_IRQHandler = Default_Handler ) ;
PROVIDE (   UART5_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM6_IRQHandler = Default_Handler ) ;
PROVIDE (   TIM7_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA2_Channel1_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA2_Channel2_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA2_Channel3_IRQHandler = Default_Handler ) ;
PROVIDE (   DMA2_Channel4_5_IRQHandler = Default_Handler ) ;




/******************************************************************************/
/*                       Peripheral memory map                                */
/******************************************************************************/
/*this allows to compile the ST lib in "non-debug" mode*/


/* Peripheral and SRAM base address in the alias region */
PERIPH_BB_BASE        = 0x42000000;
SRAM_BB_BASE          = 0x22000000;

/* Peripheral and SRAM base address in the bit-band region */
SRAM_BASE             = 0x20000000;
PERIPH_BASE           = 0x40000000;

/* Flash registers base address */
PROVIDE ( FLASH_BASE            = 0x40022000);
/* Flash Option Bytes base address */
PROVIDE ( OB_BASE               = 0x1FFFF800);

/* Peripheral memory map */
APB1PERIPH_BASE      = PERIPH_BASE  ;
APB2PERIPH_BASE      = (PERIPH_BASE + 0x10000) ;
AHBPERIPH_BASE       = (PERIPH_BASE + 0x20000) ;

PROVIDE ( TIM2            = (APB1PERIPH_BASE + 0x0000) ) ;
PROVIDE ( TIM3            = (APB1PERIPH_BASE + 0x0400) ) ;
PROVIDE ( TIM4            = (APB1PERIPH_BASE + 0x0800) ) ;
PROVIDE ( TIM6            = (APB1PERIPH_BASE + 0x1000) ) ;
PROVIDE ( TIM7            = (APB1PERIPH_BASE + 0x1400) ) ;
PROVIDE ( RTC             = (APB1PERIPH_BASE + 0x2800) ) ;
PROVIDE ( WWDG            = (APB1PERIPH_BASE + 0x2C00) ) ;
PROVIDE ( IWDG            = (APB1PERIPH_BASE + 0x3000) ) ;
PROVIDE ( SPI2            = (APB1PERIPH_BASE + 0x3800) ) ;
PROVIDE ( USART2          = (APB1PERIPH_BASE + 0x4400) ) ;
PROVIDE ( USART3          = (APB1PERIPH_BASE + 0x4800) ) ;
PROVIDE ( I2C1            = (APB1PERIPH_BASE + 0x5400) ) ;
PROVIDE ( I2C2            = (APB1PERIPH_BASE + 0x5C00) ) ;
PROVIDE ( BKP             = (APB1PERIPH_BASE + 0x6C00) ) ;
PROVIDE ( PWR             = (APB1PERIPH_BASE + 0x7000) ) ;
PROVIDE ( DAC             = (APB1PERIPH_BASE + 0x7400) ) ;
PROVIDE ( CEC             = (APB1PERIPH_BASE + 0x7800) ) ;

PROVIDE ( AFIO            = (APB2PERIPH_BASE + 0x0000) ) ;
PROVIDE ( EXTI            = (APB2PERIPH_BASE + 0x0400) ) ;
PROVIDE ( GPIOA           = (APB2PERIPH_BASE + 0x0800) ) ;
PROVIDE ( GPIOB           = (APB2PERIPH_BASE + 0x0C00) ) ;
PROVIDE ( GPIOC           = (APB2PERIPH_BASE + 0x1000) ) ;
PROVIDE ( GPIOD           = (APB2PERIPH_BASE + 0x1400) ) ;
PROVIDE ( GPIOE           = (APB2PERIPH_BASE + 0x1800) ) ;
PROVIDE ( ADC1            = (APB2PERIPH_BASE + 0x2400) ) ;
PROVIDE ( TIM1            = (APB2PERIPH_BASE + 0x2C00) ) ;
PROVIDE ( SPI1            = (APB2PERIPH_BASE + 0x3000) ) ;
PROVIDE ( USART1          = (APB2PERIPH_BASE + 0x3800) ) ;
PROVIDE ( TIM15           = (APB2PERIPH_BASE + 0x4000) ) ;
PROVIDE ( TIM16           = (APB2PERIPH_BASE + 0x4400) ) ;
PROVIDE ( TIM17           = (APB2PERIPH_BASE + 0x4800) ) ;

PROVIDE ( DMA             = (AHBPERIPH_BASE + 0x0000) ) ;
PROVIDE ( DMA_Channel1    = (AHBPERIPH_BASE + 0x0008) ) ;
PROVIDE ( DMA_Channel2    = (AHBPERIPH_BASE + 0x001C) ) ;
PROVIDE ( DMA_Channel3    = (AHBPERIPH_BASE + 0x0030) ) ;
PROVIDE ( DMA_Channel4    = (AHBPERIPH_BASE + 0x0044) ) ;
PROVIDE ( DMA_Channel5    = (AHBPERIPH_BASE + 0x0058) ) ;
PROVIDE ( DMA_Channel6    = (AHBPERIPH_BASE + 0x006C) ) ;
PROVIDE ( DMA_Channel7    = (AHBPERIPH_BASE + 0x0080) ) ;
PROVIDE ( RCC             = (AHBPERIPH_BASE + 0x1000) ) ;

/* System Control Space memory map */
SCS_BASE              = 0xE000E000;

PROVIDE ( SysTick         = (SCS_BASE + 0x0010) ) ;
PROVIDE ( NVIC            = (SCS_BASE + 0x0100) ) ;
PROVIDE ( SCB             = (SCS_BASE + 0x0D00) ) ;

/* Memory Spaces Definitions */
MEMORY
{
  IRQ_TBL (rx)		 : ORIGIN = 0x08000000, LENGTH = 2K
  APPLIC (rx)        : ORIGIN = 0x08000800, LENGTH = 94K
  BAT_DATA (r)       : ORIGIN = 0x08018000, LENGTH = 1K
  MFG_DATA (r)		 : ORIGIN = 0x08018400, LENGTH = 1K 
  EXTDATA (r)		 : ORIGIN = 0x08018800, LENGTH = 30K
  
  RAM (xrw)          : ORIGIN = 0x20000000, LENGTH = 8K
}
 
/* higher address of the user mode stack */
_estack = 0x20002000;

_BatDataSectionStartAdr = ORIGIN(BAT_DATA);
_BatDataSectionEndAdr   = ORIGIN(BAT_DATA) + LENGTH(BAT_DATA) - 1;
_MfgDataSectionStartAdr = ORIGIN(MFG_DATA);
_MfgDataSectionEndAdr   = ORIGIN(MFG_DATA) + LENGTH(MFG_DATA) - 1;
_ExtDataSectionStartAdr = ORIGIN(EXTDATA);
_ExtDataSectionEndAdr 	= ORIGIN(EXTDATA) + LENGTH(EXTDATA) - 1;

/* Sections Definitions */
SECTIONS
{
    .app_rst :
    {
        . = ALIGN(4);
        KEEP(*(.app_rst))
        . = ALIGN(4);
    } >IRQ_TBL

  	.bat_data :
    {
        . = ALIGN(4);
        KEEP(*(.bat_data))              /* rechargeable battery data */
        . = ALIGN(4);
    } >BAT_DATA

  	.mfg_data :
    {
        . = ALIGN(4);
        KEEP(*(.mfg_data))              /* manufacturing data */
        . = ALIGN(4);
    } >MFG_DATA

    /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
        . = ALIGN(4);
        _sapp_info = . ;
        KEEP(*(.app_info))
        KEEP(*(.app_start_str))

        . = ALIGN(0x200);
        _sisr_vector = . ;
        KEEP(*(.isr_vector))

        . = ALIGN(4);
        *(.text)                   /* remaining code */
        *(.text.*)                 /* remaining code */
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        KEEP(*(.app_stop_str))
        . = ALIGN(4);
        _etext = .;
    } >APPLIC

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } >APPLIC

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } >APPLIC
    __exidx_end = .;


     /* This is used by the startup in order to initialize the .data secion */
     _sidata = .;


    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data  : AT ( _sidata )
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _sdata = . ;
        *(.data)
        *(.data.*)
        *(.RAMtext)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _edata = . ;
    } >RAM



    /* This is the uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;

        *(.bss)
        *(COMMON)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _ebss = . ;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );

    /* This is the user stack section
    This is just to check that there is enough RAM left for the User mode stack
    It should generate an error if it's full.
     */
    ._usrstack :
    {
        . = ALIGN(4);
        _susrstack = . ;

        . = . + _Minimum_Stack_Size ;

        . = ALIGN(4);
        _eusrstack = . ;
    } >RAM


    /* after that it's only debugging information. */

    /* remove the debugging information from the standard libraries */
    /DISCARD/ :
    {
     libc.a ( * )
     libm.a ( * )
     libgcc.a ( * )
     }

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
