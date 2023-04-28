/** Description        *******************************************************************************/
/** #INCLUDES          *******************************************************************************/
#include <stdio.h>
#include "system.h"
#include "globals.h"
#include "syslib.h"
#include "task.h"
#include "sage.h"

/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/
/** Public Functions   *******************************************************************************/

/** Private Variables  *******************************************************************************/

/** Private Functions  *******************************************************************************/
/** Code               *******************************************************************************/

void NMI_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQNMI, FALSE ); }

void HardFault_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQHARDFAULT, FALSE ); }

void MemManage_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQMEMMANAGE, FALSE ); }

void BusFault_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQBUSFAULT, FALSE ); }

void UsageFault_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQUSAGEFAULT, FALSE ); }

void SVC_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQSVC, FALSE ); }

void DebugMon_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQDEBUGMON, FALSE ); }

void PendSV_Handler( void )
{ ErrorBlink( ERR_BLINK_IRQPENDSV, FALSE ); }

/******************************************************************************************************
                                            SysTick_Handler
-------------------------------------------------------------------------------------------------------
SysTick IRQ Handler
******************************************************************************************************/
void SysTick_Handler( void )
{
   sys_tic += 10;
   SetTaskState( SAGE_TASK, SageTsk_CheckForAction );
   ExitLowPowerState();
}

void WWDG_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void PVD_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TAMPER_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void RTC_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void FLASH_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void RCC_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

//Verwendet für BQ24192-Interrupt (I/O)
//void EXTI0_IRQHandler( void )
//{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI1_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI2_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI3_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI4_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI5_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI6_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI7_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI8_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI9_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI15_10_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI14_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI15_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void DMA1_Channel1_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void DMA1_Channel2_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void DMA1_Channel3_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

//Verwendet für serielle Kommunikation. Zu finden in serial.c
//void DMA1_Channel4_IRQHandler( void )
//{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

//Verwendet für serielle Kommunikation. Zu finden in serial.c
//void DMA1_Channel5_IRQHandler( void )
//{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void DMA1_Channel6_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void DMA1_Channel7_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

// Verwendet für ADC-Wandler. Zu finden in syslib.c
//void ADC1_IRQHandler( void )
//{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void EXTI9_5_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM1_BRK_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM1_UP_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM1_TRG_COM_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM1_CC_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM2_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM3_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM4_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void I2C1_EV_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void I2C1_ER_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void I2C2_EV_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void I2C2_ER_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void SPI1_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void SPI2_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

//Verwendet für serielle Kommunikation. Zu finden in serial.c
//void USART1_IRQHandler( void )
//{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void USART2_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void USART3_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void RTCAlarm_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void CEC_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM12_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM13_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM14_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void FSMC_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM5_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void SPI3_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void UART4_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void UART5_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM6_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }

void TIM7_IRQHandler( void )
{ ErrorBlink( ERR_BLINK_IRQUNUSED, FALSE ); }
