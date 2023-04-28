// Um ein einzelnes C File bzw Teile davon für Debugzwecke nicht zu optimieren kann man den
// betreffenden Sourcecode mit den Zeilen
// #pragma GCC optimize 0
// #pragma GCC reset_options
// umgeben. Wichtig ist das die geänderte Optimierung mit dem reset_options wieder resettet wird!

/**********   #INCLUDES            **********/
#include "system.h"
#include "globals.h"
#include "syslib.h"
#include "sage.h"
#include "task.h"
#include "serial.h"
#include "crc.h"
#include "flash.h"
#include "power.h"
#include "I2C.h"
#include "info.h"

/**********   #DEFINES             **********/
/**********   ENUMS                **********/
/**********   STRUCTS              **********/
/**********   CONSTS               **********/
/**********   Private Variables    **********/
u16 AkkuThreas[100];
u8  AkkuIntRes[100];
BatStateStruct BatState;

/**********   Private Functions    **********/
void DeInitCpu( void );

/**********   Code                 **********/

/******************************************************************************************************
                                                 main
******************************************************************************************************/
int main( void ) 
{
	u16 loop;
	u16 AkkuState = 0;
	u32 AkkuMittelwert = 0;
	u32 AltAkkuMittelwert = 0;
	//u32 AMW[28];
	u8 LedL, LedLm, LedMh, LedH;

	// Initialisierung
	DeInitCpu();								// alle Register zuruecksetzen
	SystemInit();								// System Init (aus Library)
	RCC_Configuration_On();						// wichtige Takte initialisieren
	RTC_Configuration();						// RTC starten, falls noch nicht geschehen

	// Der SysTick Timer wird zur Erzeugung von 10ms Interrupts verwendet. Der Timer läuft mit
	// 1MHz. Reloadvalue auf 9999 (10000 - 1) ergibt 10ms. Höchste Priorität damit andere IRQs
	// zuverlässig auf Delay-Routinen zugreifen können.
	SysTick->LOAD = SYSTICK_RELOAD_VALUE;
	NVIC_SetPriority( SysTick_IRQn, 0 );
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000003;

	// Interrupt-Initialisierung
	NVIC_Configuration();						// NVIC Configuration
	__enable_irq();
	__enable_fault_irq();

	GpioInit();									// Power IO-Initialisierung
	PowerState = POWER_UNKNOWN;					// Grundstatus
	ChargerType = CHRG_NODETECT;				// Grundeinstellung
	PowerInit();								// erst mal alles abschalten außer 3V3 und 5V

	// Tasks initialisieren
	InitTaskList();
	SetTaskState( SAGE_TASK, SageTsk_Init );
	DispBacklight.Mode = 1;
	InitLedState(); RgbLedState.GeneralBrightness = 1001;
	if (DET_TEMPPWROFF) SetLedState( LED_RGB4_RED, 500, 0, 0, 1500, 100, 100 );

	// Akku-Spannungs- und Widerstandswerte aus dem Flash lesen. Wenn nicht vorhanden: Standardwerte
/*	if ( (BatData->mfgmagic1 == 0x01020304) && (BatData->mfgmagic2 == 0x04030201) )
	{
		// wenn Kapazität mehr als 30% vom erwarteten Wert (22 Ah) abweicht: Standardwerte verwenden
		if ( (BatData->calib_capacity < 15400) || (BatData->calib_capacity > 28600) )
		{
			for (loop=0; loop<100; loop++)
			{
				AkkuThreas[loop] = StdAkkuThreas[loop];
				AkkuIntRes[loop] = StdAkkuIntRes[loop];
			}
			BatState.IntCalibState = CALIB_NOK;
		}
		// wenn Kapazität mehr als 15% vom erwarteten Wert (22 Ah) abweicht: Standardwerte dazumitteln
		else if ( (BatData->calib_capacity < 18700) || (BatData->calib_capacity > 25300) )
		{
			for (loop=0; loop<100; loop++)
			{
				AkkuThreas[loop] = (BatData->voltage_array[99-loop] + StdAkkuThreas[loop]) >> 1;
				AkkuIntRes[loop] = ((u8)BatData->res_array[99-loop] + StdAkkuIntRes[loop]) >> 1;
			}
			BatState.IntCalibState = CALIB_DONE;
		}
		// wenn der Messwert maximal 15% von 22 Ah abweicht: Messwerte verwenden
		else
		{
			for (loop=0; loop<100; loop++)
			{
				AkkuThreas[loop] = BatData->voltage_array[99-loop];
				AkkuIntRes[loop] = (u8)BatData->res_array[99-loop];
			}
			BatState.IntCalibState = CALIB_DONE;
		}
	} 
	// keine Messwerte vorhanden: Standardwerte verwenden
	else */
	{
		for (loop=0; loop<100; loop++)
		{
			AkkuThreas[loop] = StdAkkuThreas[loop];
			AkkuIntRes[loop] = StdAkkuIntRes[loop];
		}
		BatState.IntCalibState = CALIB_NOT_DONE;
	}

	// LED-Akkuanzeige vor dem Einschalten	
	for (loop=0; loop<10; loop++)
	{
		DelayUsRaw (10000);
		DoSysAdMeasurements();
		AnalogVals.Akku_mA = -200;
		GetBatLevel();
		AkkuState += BatState.IntChrgState;
	}

	AkkuState /= 10;
	AltAkkuMittelwert = AkkuState;
	//DbgPrintf ("Akkumittelwert 0: %d", AkkuState);
	LedL = 0; LedLm = 0; LedMh = 0; LedH = 0;
	
	for (loop=1; loop<28; loop++)
	{
		DelayUsRaw (50000);
		DoSysAdMeasurements();
		AnalogVals.Akku_mA = -200;
		GetBatLevel();
		AkkuState += BatState.IntChrgState; AkkuMittelwert = AkkuState / (loop + 1); 
		if (((AltAkkuMittelwert-AkkuMittelwert) > 3) || ((AkkuMittelwert - AltAkkuMittelwert) > 3)) 
			AkkuMittelwert = (AltAkkuMittelwert + AkkuMittelwert) >> 1;
		SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
		
		if (loop==8)
		{
			CheckExtPower();
			if ( (ChargerType == CHRG_EXTNT_12V) || (ChargerType == CHRG_EXTNT_9V) ) SetExtPowerMode();
			OvercurrentTest();
		}

		if ( (ChargerType == CHRG_NOCHARGER) || (ChargerType == CHRG_NODETECT) )
		{
			if (AkkuMittelwert < EMPTY) { SetLedState( LED_RGB1_RED, 500, 0, 0, 200, 200, 0 ); LedL++; }
			else if (AkkuMittelwert < LOWMED) { SetLedState( LED_RGB1_YELLOW, 40, 460, 0, 200, 200, 0 ); LedLm++; }
			else if (AkkuMittelwert < MEDHIGH) { SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW, 40, 460, 0, 200, 200, 0 ); LedMh++; }
			else { SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW | LED_RGB3_YELLOW, 40, 460, 0, 200, 200, 0 ); LedH++; }
			if ((LedLm > 4) && (LedMh < 4)) SetLedState( LED_RGB1_YELLOW, 40, 460, 0, 200, 200, 0 );
			if ((LedMh > 4) && (LedH  < 4)) SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW, 40, 460, 0, 200, 200, 0 );
			if ((LedH  > 4) && (LedLm < 4)) SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW | LED_RGB3_YELLOW, 40, 460, 0, 200, 200, 0 );
		}
		
		SetDispLedBrightness( AUTOLEDS, 0, 0 );
		AltAkkuMittelwert = AkkuMittelwert;
		//AMW[loop] = AkkuMittelwert;
	}

	if (0) 	// nur zu Testzwecken!!!
	{
		u32 Seconds;
		RTC_GetTime( &Seconds );
		DbgPrintf ("Zeit: %d Sec seit 1.1.70 | Battstatus: %d", Seconds, AkkuMittelwert);
		//for (loop=1; loop<27; loop+=3) DbgPrintf ("AMW[%d] = %d  | AMW[%d] = %d ", loop, AMW[loop], loop+1, AMW[loop+1]);
	}

	// Modulmasken aus dem Flash lesen. Falls keine Werte vorhanden: Standardwerte setzen
	HwModuleMask.Available = MfgData->available_mask;
	if (HwModuleMask.Available == 0xFFFF) HwModuleMask.Available = 0x067F;
	HwModuleMask.Usable = MfgData->activated_mask;
	if (HwModuleMask.Usable == 0xFFFF) HwModuleMask.Usable = 0x067F;

	// Endlosschleife
	TaskLoopEndless();

	return 0;
}

/******************************************************************************************************
                                              DeInitCpu
-------------------------------------------------------------------------------------------------------
Alle Register auf Defaults
******************************************************************************************************/
void DeInitCpu( void ) 
{
	ADC_DeInit( ADC1 );
	BKP_DeInit();
	DMA_DeInit( DMA1_Channel1 );
	DMA_DeInit( DMA1_Channel2 );
	DMA_DeInit( DMA1_Channel3 );
	DMA_DeInit( DMA1_Channel4 );
	DMA_DeInit( DMA1_Channel5 );
	DMA_DeInit( DMA1_Channel6 );
	DMA_DeInit( DMA1_Channel7 );
	EXTI_DeInit();
	GPIO_DeInit( GPIOA );
	GPIO_DeInit( GPIOB );
	GPIO_DeInit( GPIOC );
	GPIO_DeInit( GPIOD );
	GPIO_DeInit( GPIOE );
	GPIO_AFIODeInit();
	I2C_DeInit( I2C1 );
	I2C_DeInit( I2C2 );
	NVIC_DeInit();
	NVIC_SCBDeInit();
	PWR_DeInit();
	RCC_DeInit();
	SPI_I2S_DeInit( SPI1 );
	SPI_I2S_DeInit( SPI2 );
	TIM_DeInit( TIM1 );
	TIM_DeInit( TIM2 );
	TIM_DeInit( TIM3 );
	TIM_DeInit( TIM4 );
	TIM_DeInit( TIM5 );
	TIM_DeInit( TIM6 );
	TIM_DeInit( TIM7 );
	USART_DeInit( USART1 );
	USART_DeInit( USART2 );
	WWDG_DeInit();
	DAC_DeInit();
}
