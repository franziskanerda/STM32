// Disable uncrustify code beautifier: *INDENT-OFF*

// Description        *******************************************************************************/
// #INCLUDES

// ACHTUNG: in dieser Version ist die Kamera immer an!!!!!
#include <stdlib.h>
#include <math.h>
#include "system.h"
#include "globals.h"
#include "syslib.h"
#include "serial.h"
#include "sage.h"
#include "power.h"
#include "I2C.h"
#include "flash.h"
#include "teles.h"

void DeInitCpu( void );

u8 ChargerType = CHRG_NODETECT;
u8 OldChargingState = 0;
static u8 WuIntReq = 0;
u8 OldChargerType = 0;
u8 UsbCheck = 1;
u8 LedTestFlag = 0;
u8 OcEvent = 0;
u16 MeasAkkuIntRes [10];
u16 AkkuCalcIntRes = 0; 
u8 AkkuArrayIndex = 0;
				

// Spannungsschwellen [mV] bei Nullbelastung des Default-Akkus
const u16 StdAkkuThreas[100] =
{
//	1%		2%		3%		4%		5%		6%		7%		8%		9%		-10%-
	3270,	3295,	3315,	3333,	3350,	3364,	3376,	3387,	3398,	3408,
//	11%		12%		13%		14%		15%		16%		17%		18%		19%		20%
	3418,	3428,	3438,	3448,	3458,	3468,	3478,	3488,	3498,	3508,
//	21%		22%		23%		24%		25%		26%		27%		28%		29%		30%
	3517,	3526,	3535,	3543,	3551,	3558,	3565,	3571,	3577,	3583,
//	31%		32%		33%		34%		35%		36%		37%		38%		39%		-40%-
	3589,	3595,	3600,	3605,	3610,	3615,	3620,	3625,	3630,	3635,
//	41%		42%		43%		44%		45%		46%		47%		48%		49%		50%
	3640,	3645,	3650,	3655,	3660,	3665,	3670,	3675,	3680,	3685,
//	51%		52%		53%		54%		55%		56%		57%		58%		59%		60%
	3690,	3696,	3702,	3708,	3715,	3722,	3729,	3737,	3746,	3755,
//	61%		62%		63%		64%		65%		66%		67%		68%		69%		-70%-
	3764,	3774,	3784,	3795,	3806,	3818,	3830,	3841,	3852,	3862,
//	71%		72%		73%		74%		75%		76%		77%		78%		79%		80%
	3872,	3881,	3890,	3899,	3908,	3916,	3924,	3931,	3938,	3944,
//	81%		82%		83%		84%		85%		86%		87%		88%		89%		90%
	3950,	3956,	3962,	3968,	3974,	3980,	3986,	3992,	3998,	4004,
//	91%		92%		93%		94%		95%		96%		97%		98%		99%		100%
	4011,	4018,	4026,	4034,	4043,	4052,	4062,	4072,	4083,	4094
};

// Angepasst fuer gemittelte Ri-Werte (Berechnete Werte und Korrekturwerte)
const u8 StdAkkuIntRes[100] =
{
//	1%		2%		3%		4%		5%		6%		7%		8%		9%		-10%-
	12,		11,		10,		10,		 9,		 9,		 8,		 8,		7,		7,
//	11%		12%		13%		14%		15%		16%		17%		18%		19%		20%
	6,		 6,		 6,		5,		5,		5,		5,		5,		5,		5,
//	21%		22%		23%		24%		25%		26%		27%		28%		29%		30%
	4,		4,		4,		4,		4,		4,		4,		4,		4,		4,
//	31%		32%		33%		34%		35%		36%		37%		38%		39%		-40%-
	4,		4,		4,		4,		4,		4,		4,		4,		4,		4,
//	41%		42%		43%		44%		45%		46%		47%		48%		49%		50%
	4,		4,		4,		4,		4,		4,		4,		4,		4,		4,
//	51%		52%		53%		54%		55%		56%		57%		58%		59%		60%
	4,		4,		4,		4,		4,		4,		3,		3,		3,		3,
//	61%		62%		63%		64%		65%		66%		67%		68%		69%		-70%-
	3,		3,		3,		3,		3,		3,		3,		3,		3,		3,
//	71%		72%		73%		74%		75%		76%		77%		78%		79%		80%
	3,		3,		3,		3,		3,		3,		3,		3,		3,		3,
//	81%		82%		83%		84%		85%		86%		87%		88%		89%		90%
	3,		3,		3,		3,		3,		3,		3,		3,		2,		2,
//	91%		92%		93%		94%		95%		96%		97%		98%		99%		100%
	2,		2,		2,		2,		2,		1,		1,		1,		0,		0
};

/******************************************************************************************************
                                          ModuleSwitch
-------------------------------------------------------------------------------------------------------
schaltet einzelne Schaltmodule ein / aus und k�mmert sich um die Masken
******************************************************************************************************/
void ModuleSwitch (void)
{
	u16 mask;

	for (mask = 0x0001; mask <= 0x0200; mask <<= 1)
	{
		switch (mask)
		{
			case 0x0001: if (HwModuleMask.OnOff & mask) { SENSOR_ON;    } else { SENSOR_OFF; 	} break;

			case 0x0002:
				if ( (HwModuleMask.OnOff & mask)
					&& (HwModuleMask.Available & 0x01)
					&& (HwModuleMask.Usable & 0x01) )
				{ CAMERA_ON; CAM_RST; }
				else
				{
					CAMERA_OFF; CAM_RST;
					if (HwModuleMask.OnOff & mask) 	// Camera not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;

			case 0x0004: if (HwModuleMask.OnOff & mask) { DISPLAY_ON;   } else { DISPLAY_OFF;   } break;
			case 0x0008: if (HwModuleMask.OnOff & mask) { TOUCH_ON;     } else { TOUCH_OFF;     } break;
			case 0x0010: if (HwModuleMask.OnOff & mask) { MICRO_SD_ON;  } else { MICRO_SD_OFF;  } break;

			case 0x0020:
				if ( (HwModuleMask.OnOff & mask)
				   && (HwModuleMask.Available & 0x08)
				   && (HwModuleMask.Usable & 0x08) )
				{ BLUETOOTH_ON; }
				else
				{
					BLUETOOTH_OFF;
					if (HwModuleMask.OnOff & mask) 	// Bluetooth not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;

			case 0x0040:
				if ( (HwModuleMask.OnOff & mask)
					&& (HwModuleMask.Available & 0x10)
					&& (HwModuleMask.Usable & 0x10) )
				{ WLAN_ON; }
				else
				{
					WLAN_OFF;
					if (HwModuleMask.OnOff & mask) 	// WLAN not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;

			case 0x0080:
				if ( (HwModuleMask.OnOff & mask)
					&& (HwModuleMask.Available & 0x04)
					&& (HwModuleMask.Usable & 0x04) )
				{ NFC_ON; }
				else
				{
					NFC_OFF;
					if (HwModuleMask.OnOff & mask) 	// NFC not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;

			case 0x0100:
				if ( (HwModuleMask.OnOff & mask)
					&& (HwModuleMask.Available & 0x20)
					&& (HwModuleMask.Usable & 0x20) )
				{ MEMS_ON; }
				else
				{
					MEMS_OFF;
					if (HwModuleMask.OnOff & mask) 	// MEMs not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;

			case 0x0200:
				if ( (HwModuleMask.OnOff & mask)
					&& (HwModuleMask.Available & 0x01)
					&& (HwModuleMask.Usable & 0x01) )
				{
					WLED_ON;
				}
				else
				{
					WLED_OFF;
					if (HwModuleMask.OnOff & mask) 	// WLED not available / usable
					{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
				}
			break;
		}
	}
	if (HwModuleMask.OnOff & 0x0002) { DelayUsRaw (1000); CAM_NRST; }
}


/******************************************************************************************************
                                          PowerInit
-------------------------------------------------------------------------------------------------------
schaltet alles in den Initialisierungsmodus (LEDs und AD-Konverter geht)
******************************************************************************************************/
void PowerInit (void)
{
	RCC_DeInit ();
	DAC_DeInit();
	PWM_Configuration ();
	InitSerial( COM_CHANNEL_RS, 115200, COM_MODE_TELE );
	I2C_Configuration ();

	CAM_RST;					// Kamera in den Reset-Zustand legen
	SW_3V3_ON;					// 3,3V-Versorgung einschalten (fuer IOs)
	SW_5V_ON;					// 5V-Versorgung ausschalten
	DelayUsRaw (2000);
	HwModuleMask.OnOff = 0x0010;// HW-Modul-Schaltmaske festlegen
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen

	SetDispLedBrightness( AUTOLEDS, 0, 0 );
	ChargerType = CHRG_NODETECT;
	OldChargingState = CHRG_NODETECT;
	OldChargerType = CHRG_NOCHARGER;
	BatState.IntChrgMode = CHRGMODE_NOCHRG;
	SETSOURCE_USB;				// Lademodus per Default immer auf USB schalten!
	SETNOCHARGE;				// Lademodus zunaechst ausschalten
	ADC_Configuration();
}


/******************************************************************************************************
                                          PowerOff
-------------------------------------------------------------------------------------------------------
schaltet alles aus
******************************************************************************************************/
void PowerOff (void)
{
	CAM_RST;					// Kamera in den Reset-Zustand legen
	SW_3V3_OFF;					// 3,3V-Versorgung ausschalten
	SW_5V_OFF;					// 5V-Versorgung ausschalten
	HwModuleMask.OnOff = 0;		// HW-Modul-Schaltmaske: alle Module aus
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen

	SETSOURCE_USB;				// Lademodus per Default immer auf USB schalten!
	SETCHARGE;					// Lademodus einschalten (spart Strom!)
	SetDispLedBrightness( NOAUTO, 0, 0 );
	I2C_SoftwareResetCmd(I2C1, ENABLE);

	I2C_DeInit( I2C1 );
	I2C_DeInit( I2C2 );
	ADC_DeInit( ADC1 );
	I2C_DeInit( I2C1 );
	TIM_DeInit( TIM1 );
	TIM_DeInit( TIM2 );
	TIM_DeInit( TIM3 );
	TIM_DeInit( TIM4 );
	USART_DeInit( USART1 );
	USART_DeInit( USART2 );
	DAC_DeInit();

	RCC_DeInit ();
	RCC_Configuration_Off();
}

/******************************************************************************************************
                                          PowerOffCharging
-------------------------------------------------------------------------------------------------------
schaltet die Module in einen Zustand, in dem geladen werden kann
******************************************************************************************************/
void PowerOffCharging (void)
{
	RCC_DeInit ();
	DAC_DeInit();
	RCC_Configuration_On();
	PWM_Configuration ();
	InitSerial( COM_CHANNEL_RS, 115200, COM_MODE_TELE );
	I2C_Configuration ();
	InitLedState();

	CAM_RST;					// Kamera in den Reset-Zustand legen
	SW_3V3_ON;					// 3,3V-Versorgung einschalten (fuer IOs)
	SW_5V_ON;					// 5V-Versorgung einschalten
	HwModuleMask.OnOff = 0x0200;// HW-Modul-Schaltmaske festlegen
  //HwModuleMask.OnOff = 0x0201;// HW-Modul-Schaltmaske festlegen
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen
	SetDispLedBrightness( AUTOLEDS, 0, 0 );

	ADC_Configuration ();
}

/******************************************************************************************************
                                          PowerOn
-------------------------------------------------------------------------------------------------------
schaltet die Module in einen Zustand, in dem der iMX6 lauffaehig ist
******************************************************************************************************/
void PowerOn (void)
{
	RCC_DeInit ();
	RCC_Configuration_On();
	PWM_Configuration ();
	InitSerial( COM_CHANNEL_RS, 115200, COM_MODE_TELE );
	I2C_Configuration ();

	while (!DET_IMX6_PGOOD);	// warten, bis iMX6 l�uft
	SW_3V3_ON;					// 3,3V-Versorgung einschalten
	SW_5V_ON;					// 5V-Versorgung einschalten
	SENSOR_OFF;					// Sensor abschalten
	HwModuleMask.OnOff = 0x031C;// HW-Modul-Schaltmaske definieren
	HwModuleMask.Error = 0;		// Fehlermaske loeschen

	DispBacklight.Mode = 0;
	SetDispLedBrightness( AUTOLEDS, 0, 0 );
	ModuleSwitch();				// HW gemaess Maske einstellen

	ADC_Configuration ();
	DAC_Configuration ();
}

/******************************************************************************************************
                                          PowerOnCharging
-------------------------------------------------------------------------------------------------------
schaltet die Module in einen Zustand, in dem der iMX6 lauffaehig ist und geladen werden kann
******************************************************************************************************/
void PowerOnCharging (void)
{
	RCC_DeInit ();
	RCC_Configuration_On();
	PWM_Configuration ();
	InitSerial( COM_CHANNEL_RS, 115200, COM_MODE_TELE );
	I2C_Configuration ();

	while (!DET_IMX6_PGOOD);	// warten, bis iMX6 laeuft
	SW_3V3_ON;					// 3,3V-Versorgung einschalten
	SW_5V_ON;					// 5V-Versorgung einschalten
	HwModuleMask.OnOff = 0x031C;// HW-Modul-Schaltmaske definieren
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen

	DispBacklight.Mode = 0;
	SetDispLedBrightness( AUTOLEDS, 0, 0 );

	ADC_Configuration ();
	DAC_Configuration ();
}

/******************************************************************************************************
                                           EXTI0_IRQHandler
-------------------------------------------------------------------------------------------------------
Dieser IRQ wird aufgerufen wenn der BQ24192 (Ladeschaltung) eine �nderung an der externen Versorgungs-
spannung erkannt hat. Der BQ24192 teilt dies �ber einen oder mehrere Pulse an Pin PE0 mit. Da der Im-
puls auch ausgel�st wird wenn ein USB-Netzteil angeschlossen wird, ist es nicht n�tig, an dem Pin des
USB3751-Bausteins ebenfallseinen Interrupt auszul�sen.
******************************************************************************************************/
void EXTI0_IRQHandler( void )
{
	EXTI_ClearITPendingBit(EXTI_Line0);
	WuIntReq = 1;
}

/******************************************************************************************************
                                          SetPowerMode
-------------------------------------------------------------------------------------------------------
Diese Funktion setzt den Betriebsmodus entsprechend der relevanten IOs.
******************************************************************************************************/
u8 GetPowerMode (void)
{
	u8 IntPower = 0;
	static u8 OldIntPower = 4;

	if (DET_PWRON != 0) IntPower |= 1;
	if (DET_BQ24192_NPGOOD == 0) IntPower |= 2;

	if (IntPower != OldIntPower)
	{
		OldChargingState = CHRG_NODETECT;
		OldChargerType = CHRG_NOCHARGER;
		BatState.IntChrgMode = CHRGMODE_NOCHRG;
		SETSOURCE_USB;				// Lademodus per Default immer auf USB schalten!
		SETNOCHARGE;				// Lademodus zunaechst ausschalten


		switch (IntPower)
		{
			case 0:
				IMX6_STM1_OFF; IMX6_STM2_OFF;
				PowerState = POWEROFF_NOTCHARGING_INIT;
			break;

			case 1:
				PowerState = POWERON_NOTCHARGING_INIT;
				if ( (OldIntPower == 2) || (OldIntPower == 3) )
				{ IMX6_STM1_ON; GlobalEvent |= 0x0100; }
			break;

			case 2:
				UsbCheck = 1;
				PowerState = POWEROFF_CHARGING_INIT;
			break;

			case 3:
				UsbCheck = 1;
				PowerState = POWERON_CHARGING_INIT;
			break;
		}
	}

	OldIntPower = IntPower;
	return PowerState;
}


/******************************************************************************************************
                                          CheckExtPower
-------------------------------------------------------------------------------------------------------
Diese Funktion prueft, was zu tun ist wenn sich etwas an der extern angeschlossenen Stromversorgung
aendert.
******************************************************************************************************/
u8 CheckExtPower (void)
{
	u8  DiffChargerDet;
	u8  UHohl = UNKNOWNVOLTAGE;
	u8  UOdu  = UNKNOWNVOLTAGE;
	u8  UUsb  = UNKNOWNVOLTAGE;

	DoSysAdMeasurements();

	if      (AnalogVals.In1_mV < 4000)	{ UHohl = UNDERVOLTAGE; }
	else if (AnalogVals.In1_mV < 7500)	{ UHohl = VOLTAGE5; }
	else if (AnalogVals.In1_mV < 10500) { UHohl = VOLTAGE9; }
	else if (AnalogVals.In1_mV < 13500) { UHohl = VOLTAGE12; }
	else 								{ UHohl = OVERVOLTAGE; }

	if      (AnalogVals.In2_mV < 4000)	{ UOdu = UNDERVOLTAGE; }
	else if (AnalogVals.In2_mV < 7500) 	{ UOdu = VOLTAGE5; }
	else if (AnalogVals.In2_mV < 10500) { UOdu = VOLTAGE9; }
	else if (AnalogVals.In2_mV < 13500) { UOdu = VOLTAGE12; }
	else 								{ UOdu = OVERVOLTAGE; }

	if      (AnalogVals.Usb_mV < I2C_MINVOLTAGE) { UUsb = UNDERVOLTAGE; }
	else if (AnalogVals.Usb_mV < 5500)  { UUsb = VOLTAGE5; }
	else 								{ UUsb = OVERVOLTAGE; }

	if ( (UHohl == VOLTAGE12) || (UOdu == VOLTAGE12) ) { ChargerType = CHRG_EXTNT_12V; UsbCheck = 1; }
	else if ( (UHohl == VOLTAGE9) || (UOdu == VOLTAGE9) ) { ChargerType = CHRG_EXTNT_9V; UsbCheck = 1; }
	else if (UUsb == VOLTAGE5) { if (UsbCheck == 1) ChargerType = CheckUsbSource(); UsbCheck = 0; }
	else if ( (UHohl == VOLTAGE5) || (UOdu == VOLTAGE5) ) { ChargerType = CHRG_EXTNT_5V; UsbCheck = 1; }
	else if ( (UHohl == UNDERVOLTAGE) && (UOdu == UNDERVOLTAGE) && (UUsb == UNDERVOLTAGE) )
	{ ChargerType = CHRG_NOCHARGER; UsbCheck = 1;}
	else { ChargerType = CHRG_DETERROR; UsbCheck = 1; }

	DiffChargerDet = ChargerType ^ OldChargerType;
	OldChargerType = ChargerType;

	if (DiffChargerDet != 0)
	{
		//DbgPrintf ("Neuer Ladestatus erkannt. Ladestatus = %X(h)", ChargerType);
		return 1;
	}
	else
	{ return 0; }
}

/******************************************************************************************************
                                          CheckUsbSource
-------------------------------------------------------------------------------------------------------
Diese Funktion prueft, ob etwas bzw. was am USB-Anschluss angeschlossen ist
******************************************************************************************************/
u8 CheckUsbSource(void)
{
	u8 UsbSource = 0, i = 0;
	s8 Error;

	I2C_Configuration ();
	DelayUsRaw (10000);
	I2C_Init_IC (USB3751);

	while ( (i<10) && (UsbSource == 0) )
	{
		DelayUsRaw (100000);
		Error = I2C_Read (USB3751, USB_STATUS, 4);
		UsbSource = I2C_Data.Usb3751.Status & 0xE0;
		i++;

		//if (Error) { DbgPrintf ("USB3751-I2C-Fehler = %d", Error); }
		//else { DbgPrintf ("USBStat: %X, USB-Conf: %X, USB-Reg2: %X, USB-Reg3: %X", I2C_Data.Usb3751.Status, I2C_Data.Usb3751.Config, I2C_Data.Usb3751.MeasContr1, I2C_Data.Usb3751.MeasContr2); }

		if (Error)
		{
			I2C_GenerateSTOP(I2C1, ENABLE);
			I2C_SoftwareResetCmd(I2C1, ENABLE);
			DelayUsRaw (10);
			I2C_SoftwareResetCmd(I2C1, DISABLE);
			I2C_Configuration ();
		}
	}

	//DbgPrintf ("USBStat: %X, USB-Conf: %X, USB-Reg2: %X, USB-Reg3: %X", I2C_Data.Usb3751.Status, I2C_Data.Usb3751.Config, I2C_Data.Usb3751.MeasContr1, I2C_Data.Usb3751.MeasContr2);
	return UsbSource;
}

/******************************************************************************************************
                                          SetExtPowerMode
-------------------------------------------------------------------------------------------------------
Diese Funktion schaltet den RunF in einen der vordefinierten Lade-Modi
******************************************************************************************************/
void SetExtPowerMode (void)
{
	s16 err = 0;
	u8 i, ChrgMode, ChgState;

	SETNOCHARGE;					// Laden erst mal abschalten

	switch (ChargerType)
	{
		case CHRG_NODETECT:			// Noch nichts erkannt
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
			ChrgMode = 0;
		break;

		case CHRG_DETERROR:			// oder falsches NT erkannt:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
			ChrgMode = 0;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_RED, 750, 0, 0, 100, 100, 0 );
			if (DET_PWRON) IMX6_STM1_ON;
		break;

		case CHRG_SDP:				// PC (5V) erkannt
		case CHRG_SE1_LOW:			// oder entsprechend schwaches USB-NT erkannt:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_500MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
			ChrgMode = 0;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_BLUE, 0, 0, 750, 100, 100, 0 );
		break;

		case CHRG_SE1_HIGH:			// oder iPhone-NT erkannt:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_900MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_1344MA;
			ChrgMode = 1;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_BLUE | LED_RGB2_BLUE, 0, 0, 750, 100, 100, 0 );
		break;

		case CHRG_CDP:				// spezielles USB-Ladegeraet mit Kommunikation
		case CHRG_DCP:				// oder Android USB-NT erkannt:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_900MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_1344MA;
			ChrgMode = 1;
			SETSOURCE_USB;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_BLUE | LED_RGB2_BLUE, 0, 0, 750, 100, 100, 0 );
		break;

		case CHRG_IPAD:				// iPad erkannt:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_1200MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_1792MA;
			ChrgMode = 1;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_BLUE | LED_RGB2_BLUE | LED_RGB3_BLUE, 0, 0, 750, 100, 100, 0 );
		break;

		case CHRG_EXTNT_5V:			// 5V-Netzteil erkannt
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_2000MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_2752MA;
			ChrgMode = 1;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_WHITE, 250, 250, 250, 100, 100, 0 );
		break;

		case CHRG_EXTNT_9V:			// 9V-Netzteil erkannt:
			if (PowerState == POWEROFF_CHARGING_INIT2)
			{
				I2C_Data.Bq24192.InpSrcContr = INP_CURR_2000MA;
				ChgState = 1;
				//SendTeleCh( TID_SETCHARGING, &ChgState, 1, COM_CHANNEL_RS );	// externes Laden
			}
			else
				I2C_Data.Bq24192.InpSrcContr = INP_CURR_1500MA;

			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_3840MA; // (zwei Akkus)
			ChrgMode = 2;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE, 250, 250, 250, 100, 100, 0 );
		break;

		case CHRG_EXTNT_12V:		// 12V-Netzteil erkannt:
			if (PowerState == POWEROFF_CHARGING_INIT2)
			{
				I2C_Data.Bq24192.InpSrcContr = INP_CURR_1500MA;
				I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_4032MA;
				ChgState = 1;
				//SendTeleCh( TID_SETCHARGING, &ChgState, 1, COM_CHANNEL_RS );	// externes Laden
			}
			else
			{
				I2C_Data.Bq24192.InpSrcContr = INP_CURR_1200MA;
				//I2C_Data.Bq24192.InpSrcContr = INP_CURR_900MA;
				I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_3520MA;
			}

			//I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_4032MA; // (zwei Akkus)
			ChrgMode = 2;
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
			err = SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 250, 250, 250, 100, 100, 0 );
		break;

		default:
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
			ChrgMode = 0;
		break;
	}

	i=0; err = 1;
	while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 1); i++; }
	i=0; err = 1;
	while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_CHAR_CURR, 1); i++; }
	if (ChrgMode < 2) { SETSOURCE_USB; }
	else { SETSOURCE_EXT; }
	if (ChrgMode == 0) { SETNOCHARGE; }
	else { SETCHARGE; }

	OldChargingState = 0;
}                          


/******************************************************************************************************
                                          CheckChargingMode
-------------------------------------------------------------------------------------------------------
Diese Funktion prueft waehrend des Ladens, ob sich was am Lademodus geaendert hat.
******************************************************************************************************/
void CheckChargingMode(void)
{
	s8 err;
	u8 ChInfo, Val1, Val2;
	static u16 OldAkkuMv = 0;
	static u16 CheckWaitTime = 0;

	do
	{
		err = I2C_Read (BQ24192, BQ_RD_STATUS, 1);
		Val1 = I2C_Data.Bq24192.SysStatus;
		DelayUsRaw (100);
		err = I2C_Read (BQ24192, BQ_INP_SRC, 11);
		Val2 = I2C_Data.Bq24192.SysStatus;
	} while (Val1 != Val2);

	DoSysAdMeasurements();

	if ( (Val1 != OldChargingState) && (DET_PWRON) )
	{ 
		GetBatLevel();
		IMX6_STM1_ON; GlobalEvent |= 0x0100; 
	}

	//DbgPrintf ("Status: %X(h), I(Akku): %d mA, U(Akku): %d mV, Fehler: %X(h), KK-Temp: %d �C, Quelle: %X(h)", I2C_Data.Bq24192.SysStatus, AnalogVals.Akku_mA, AnalogVals.Akku_mV, I2C_Data.Bq24192.SysFault, AnalogVals.TempKuehlDeg, ChargerType);

	if (I2C_Data.Bq24192.SysStatus == 0) SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );

	if ( (I2C_Data.Bq24192.SysStatus != OldChargingState) || ( ( AnalogVals.Akku_mV - OldAkkuMv ) > 25 ) )
	{
		OldAkkuMv = AnalogVals.Akku_mV;

		// Uebertemperatur anzeigen (LED blinkt unregelmaessig)
		if (I2C_Data.Bq24192.SysStatus & 0x02)
		{
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );

			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );

			if (BatState.IntChrgState < LOWMED)
			{ SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 500, 1000 ); }
			else if (BatState.IntChrgState < MEDHIGH)
			{
				SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
				SetLedState( LED_RGB2_CYAN, 0, 450, 50, 1000000000, 500, 1000 );
			}
			else
			{
				SetLedState( LED_RGB1_CYAN | LED_RGB2_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
				SetLedState( LED_RGB3_CYAN, 0, 450, 50, 1000000000, 500, 1000 );
			}
			BatState.IntChrgMode = CHRGMODE_THERMREG;
		}
		else
		{
			ChInfo = (I2C_Data.Bq24192.SysStatus & 0x30) >> 4;

			switch (ChInfo)
			{
				case 1:
					BatState.IntChrgMode = CHRGMODE_PRECHRG;
					SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
					if (BatState.IntChrgState < LOWMED)
					{ SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 1000, 1000 ); }
					else if (BatState.IntChrgState < MEDHIGH)
					{
						SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
						SetLedState( LED_RGB2_CYAN, 0, 450, 50, 1000000000, 1000, 1000 );
					}
					else
					{
						SetLedState( LED_RGB1_CYAN | LED_RGB2_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
						SetLedState( LED_RGB3_CYAN, 0, 450, 50, 1000000000, 1000, 1000 );
					}
				break;

				case 2:
					BatState.IntChrgMode = CHRGMODE_FAST;
					SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
					if (BatState.IntChrgState < LOWMED)
					{ SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 500, 500 ); }
					else if (BatState.IntChrgState < MEDHIGH)
					{
						SetLedState( LED_RGB1_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
						SetLedState( LED_RGB2_CYAN, 0, 450, 50, 1000000000, 500, 500 );
					}
					else
					{
						SetLedState( LED_RGB1_CYAN | LED_RGB2_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
						SetLedState( LED_RGB3_CYAN, 0, 450, 50, 1000000000, 500, 500 );
					}
				break;

				case 3:
					BatState.IntChrgMode = CHRGMODE_DONE;
					SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
					SetLedState( LED_RGB1_CYAN | LED_RGB2_CYAN | LED_RGB3_CYAN, 0, 450, 50, 1000000000, 1000000000, 0 );
				break;

				default:
					BatState.IntChrgMode = CHRGMODE_NOCHRG;
					if ( (ChargerType == CHRG_SDP) || (ChargerType == CHRG_SE1_LOW) )
					{
						SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );
						SetLedState( LED_RGB1_BLUE, 0, 0, 500, 1000000000, 500, 2500 );
						if (BatState.IntChrgState < ABS_EMPTY)
						{  SetLedState( LED_RGB1_RED, 500, 0, 0, 1000000000, 50, 450 );  }
					}
				break;
			}
		}
	}
	
	// wenn sinnvoller Akku-Innenwiderstand ermittelt werden kann: Durchfuehren, Mittelwert ins Flash schreiben
	if ((BatState.IntChrgMode == CHRGMODE_FAST)  && (AnalogVals.Akku_mV_LT > 3500)  && (AnalogVals.Akku_mA < -1500) && (AkkuArrayIndex < 10) && (CheckWaitTime == 50))
	{
		s16 ChargeCurrent, NoChargeCurrent, ChargeVoltage, NoChargeVoltage;
		float FloatResAkku, DiffVoltage, DiffCurrent;
		u16 AkkuIntResNr;
		
		// Messung beim Laden
		ChargeCurrent = AnalogVals.Akku_mA;
		ChargeVoltage = AnalogVals.Akku_mV;
		
		// Messung mit abgeschaltetem Laden
		SETNOCHARGE;
		DelayMs (200);
		DoSysAdMeasurements();
		NoChargeCurrent = AnalogVals.Akku_mA;
		NoChargeVoltage = AnalogVals.Akku_mV;
		
		// Laden wieder einschalten und Differenzrechnung
		SETCHARGE;
		DiffVoltage = (float)(ChargeVoltage - NoChargeVoltage);
		DiffCurrent = (float)(NoChargeCurrent - ChargeCurrent);
		FloatResAkku = (1000.0 * DiffVoltage) / DiffCurrent;
		AkkuIntResNr = 0; 				
		MeasAkkuIntRes[AkkuArrayIndex] = (u16)FloatResAkku;
		//SetLedState( LED_RGB4_GREEN, 0, 500, 0, 500, 500, 0 );
		CheckWaitTime++;
		
		// bei letztem Messwert: Werte aufaddieren
		if (AkkuArrayIndex == 9) 
		{	
			u16 i;
			
			for (i=0; i<10; i++)
			{
				if ((MeasAkkuIntRes[i] > 40) && (MeasAkkuIntRes[i] < 140))
				{
					AkkuCalcIntRes += MeasAkkuIntRes[i];
					AkkuIntResNr++;
				}
			}
	
			// Wenn genuegend sinnvolle Werte vorhanden sind: Speichern im Flash
			if (AkkuIntResNr > 4)
			{		
				BatDataStrct BatteryData;
				//SetLedState( LED_RGB4_GREEN, 0, 500, 0, 500, 500, 0 );		
				AkkuCalcIntRes /= AkkuIntResNr;
				if (BatState.IntChrgState < 100) AkkuCalcIntRes -= StdAkkuIntRes[BatState.IntChrgState];
				BatteryData.mfgmagic1 = 0x01020304;
				BatteryData.mfgmagic2 = 0x04030201;
				BatteryData.calib_unix_time = BatData->calib_unix_time;
				BatteryData.calib_capacity = BatData->calib_capacity;
				for (i=0; i<100; i++)
				{ BatteryData.res_array[i] = 0; BatteryData.voltage_array[i] = 0; }
				BatteryData.res_array[0] = (u16)AkkuCalcIntRes;
				EraseFlash( (void*)BatData, (void *)BatData + 512);
				WriteFlash( &BatteryData, (void *)BatData, 512);
			}
			else
			{	AkkuCalcIntRes = 0; 	}
		
		}
		
		AkkuArrayIndex++;
	}					
	else
	{ 
		if (BatState.IntChrgMode == CHRGMODE_FAST) 
			CheckWaitTime++; 
		else
			CheckWaitTime = 0;
		
		if (CheckWaitTime > 50) CheckWaitTime = 0; 
	}
	
	OldChargingState = I2C_Data.Bq24192.SysStatus;
}


/******************************************************************************************************
                                          GetBatLevel
-------------------------------------------------------------------------------------------------------
Diese Funktion bestimmt mit Hilfe der gemessenen Akkuspannung, des Akkustroms sowie durch die Verwendung
Akkuspezifischenr oder Allgemeiner Tabellen f�r die Zuordnung von Akkunulllastspannung/Level bzw.
Akkuwiderstand/Level den Akkuladezustand. Dazu wird zun�chst die Akkuspannung gemessen und mit Hilfe
der Innenwiderstandstabelle grob der Innenwiderstand des Akkus bestimmt. Der Innenwiderstand multipliziert
mit dem gemessenen Akkustrom ergibt die Spannungsdifferenz �ber den Akku-Innenwiderstand, daraus kann die
Nulllastspannung beim gegebenen Lastfall berechnet werden. Mit Hilfe dieser Spannung und der Nulllastta-
belle kann der Index und damit der Ladezustand des Akkus bestimmt werden. Zur Erh�hung der Genauigkeit
kann mit dem neuen Index erneut der Innenwiderstand, die Nulllastspannung und damit ein korrigierter
Ladezustand berechnet werden.
******************************************************************************************************/
void GetBatLevel(void)
{
	extern u16 AkkuThreas[];
	extern u8  AkkuIntRes[];

	u16 i, AV, AVL, AVC;
	volatile u16 jx;
	s16 AI, ChrgDiff, OldChrgState = 0;
	u16 AkkuIntResSum = 0, ChrgStateSum = 0;
	static u16 ChargeStateArray[64];
	static u8 ArrayLoop = 0;
	AV = AnalogVals.Akku_mV;
	AVL = AnalogVals.Akku_mV_LT;
	AI = AnalogVals.Akku_mA;
	i = 0;

	// Verwenden der Langzeitwerte wenn moeglich
	if ( (AV - AVL) < 100)
	{ AV = AVL; AI = AnalogVals.Akku_mA_LT; }
	
	// Widerstandswert aus Flash verwenden wenn verfuegbar; ansonsten: Tabellenwert + 60 (Offset)
	AkkuCalcIntRes = BatData->res_array[0];
	if ((AkkuCalcIntRes < 40) && (AkkuCalcIntRes > 140)) 
		AkkuCalcIntRes = 60;

	// mehrstufige Tabellenabfrage
	while ( (AV >= AkkuThreas[i]) && (i < 99) ) i++;

	AkkuIntResSum = AkkuCalcIntRes + AkkuIntRes[i];
	AVC = AV + ( AkkuIntResSum * AI / 1000 );
	
	i=0;
	while ( (AVC >= AkkuThreas[i]) && (i < 99) ) i++;
	
	AkkuIntResSum = AkkuCalcIntRes + AkkuIntRes[i];
	AVC = AV + ( AkkuIntResSum * AI / 1000 );
	
	i=0;
	while ( (AVC >= AkkuThreas[i]) && (i < 99) ) i++;
	if (AVC >= AkkuThreas[99]) i=100;
	BatState.IntChrgState = i;

	// Messwert in Array speichern; Array hat 64 Elemente
	ChargeStateArray[ArrayLoop] = BatState.IntChrgState;
	if ( (AI < 0) && (ChargeStateArray[ArrayLoop] < 100) ) ChargeStateArray[ArrayLoop] += 1;
	ArrayLoop = (ArrayLoop + 1) & 0x3F;
	ChrgStateSum = 0;
	jx = 0;

	// Maximal 64 Werte aufaddieren und Durchschnitt bilden
	while ( (ChargeStateArray[jx] > 0) && (jx<64) )
	{
		ChrgStateSum += ChargeStateArray[jx]; 
		jx++;
	} 	
	if (ChrgStateSum > 0) BatState.IntChrgState = ChrgStateSum / jx;
	
	// Akkuflags setzen
	if ( (DET_PWRON) && (AI>0) )
	{
		if (i < ABS_EMPTY)
		{ IMX6_STM1_ON; IMX6_STM3_ON; GlobalEvent |= 0x1000; }
		else
		{ IMX6_STM2_OFF; }

		if ( (i < EMPTY) && (i >= ABS_EMPTY) )
		{ IMX6_STM1_ON; GlobalEvent |= 0x0800; }
	}

	// Bei groesserer laengerfristiger Aenderung: Event senden 
	ChrgDiff = (s16) (i - OldChrgState);
	if ( ( ChrgDiff < -4 ) || ( ChrgDiff > 4 ) )
	{
		OldChrgState = i;
		if (DET_PWRON)
		{ IMX6_STM1_ON; GlobalEvent |= 0x0400; }
	}
	
	//LastChargeState = BatState.IntChrgState;
	//SetLedState( LED_RGB4_BLUE, 0, 0, 500, 100, 100, 0 );
}

/******************************************************************************************************
                                          KeyTest
-------------------------------------------------------------------------------------------------------
Diese Funktion prueft, was zu tun ist wenn die Ein/Ausschalttaste gedrueckt wird. Die Funktion wartet
bei gedrueckter Taste solange, bis die Taste wieder losgelassen wurde.
******************************************************************************************************/
void KeyTest (void)
{
	static u8 KeyPressed = 0;
	static u8 KeyUnpressed = 0;
	static u8 MultiplePressed = 0;

	// Analyse des Tastenzustands
	if (DET_ONOFF_KEY)								// Taste gedrueckt...
	{
		if ((KeyPressed>0) && (KeyPressed<100))		// KeyPressed hochzaehlen und KeyUnpressed zuruecksetzen
			KeyPressed++;
		KeyUnpressed = 1;
	}
	else 											// Taste nicht gedrueckt:
	{
		if (KeyUnpressed < 100) 					// KeyUnpressed hochzaehlen und KeyPressed zuruecksetzen
			KeyUnpressed++;
		KeyPressed = 1;
	}

	// nach 50 mSec Tastendruck: Tastendruckzaehler inkrementieren
	if (KeyPressed == 5)
	{
		MultiplePressed++;							// Tastendruckzaehler inkrementieren
		Beep (2000, 50);
	}

	// nach 400 mSec ohne Taste: Tastendruckzaehler zuruecksetzen
	if (KeyUnpressed == 40) MultiplePressed = 0;
}

/******************************************************************************************************
                                          OvercurrentTest
-------------------------------------------------------------------------------------------------------
Diese Funktion prueft, ob eine oder mehrere Hardware-Komponenten einen �berstrom melden. Ausserdem wird
geprueft, ob eine der Temperaturwarnschwellen am K�hlk�rper ausgel�st wurde. Falls eine der Bedingungen
erfuellt ist, wird das per LEDs angezeigt, zus�tzlich wird die Leitung 'IMX6_STM1' ausgeloest.
******************************************************************************************************/
void OvercurrentTest (void)
{
	u8 OcVar = DET_OVERCURRENT;

	if ( (OcVar > 0) || DET_TEMPPWROFF )
		SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 );

	if (OcVar | OC_NFC)
		if (DET_OC_NFC) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 350, 25, 175 );
	if (OcVar | OC_SD_MEMS)
		if (DET_OC_SD_MEMS) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 550, 25, 175 );
	if (OcVar | OC_FUNK)
		if (DET_OC_FUNK) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 750, 25, 175 );
	if (OcVar | OC_DISP_TOUCH)
		if (DET_OC_DISP_TOUCH) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 950, 25, 175 );
	if (OcVar | OC_SENS)
		if (DET_OC_SENS) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 1150, 25, 175 );
	if (OcVar | OC_IMX6)
		if (DET_OC_IMX6) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 1350, 25, 175 );
	if (OcVar | OC_WLED)
		if (DET_OC_WLED) SetLedState( LED_RGB4_YELLOW, 420, 80, 0, 25, 25, 0 );

	if (DET_TEMPPWROFF) SetLedState( LED_RGB4_RED, 500, 0, 0, 500, 500, 500 );

	if ( (OcVar > 0) && (DET_PWRON) )
	{ IMX6_STM1_ON; GlobalEvent |= 0x01; OcEvent |= OcVar; }
}

/******************************************************************************************************
                                           SoftReset
-------------------------------------------------------------------------------------------------------
Resettet den RunF
******************************************************************************************************/
void SoftReset( void )
{
   u32  UsartRegData;
   void (*reset)( void );
   UsartRegData = USART2->BRR;
   DeInitCpu();
   USART2->BRR = UsartRegData;
   __disable_irq();
   __disable_fault_irq();
   reset = ( void( * ) ( void ) )*(u32 *)4; // Adresse der ResetHandlers steht an Adresse 4
   reset();
}

/******************************************************************************************************
                                     BatteryMeasurement
-------------------------------------------------------------------------------------------------------
Misst die Akkukapazitaet und zeichnet Messkurven f�r die Nullstromwerte und Innenwiderst�nde auf. Aus
diesen Tabellen werden bei spaeteren Messungen die Akkuzustaende aus den Strom/Spannungswerten be-
stimmt. Der Akku wird vor der eigentlichen Messung vollgeladen und nach der Messung wieder auf ca. 60%
geladen. Diese Funktion braucht mit dem normalen Ladeger�t bis zu 24 Stunden.
-------------------------------------------------------------------------------------------------------
                                 1) BM_FullCharging
------------------------------------------------------------------------------------------------------*/
void BM_FullCharging( void )
{
	u8  DoneCount, i, Tm4;
	u32 StoppTime;
	s16 err = 0;

	// Strom sparen
	SW_3V3_ON;					// 3,3V-Versorgung einschalten
	SW_5V_ON;					// 5V-Versorgung einschalten
	HwModuleMask.OnOff = 0x031C;// HW-Modul-Schaltmaske definieren
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen
	FlashLedState.ModeMask = 0x0; SetFlashLedMode ();
	I2C_Configuration ();
	DelayUsRaw (100000);
	I2C_Init_IC (BQ24192);
	SETSOURCE_EXT;
	SETCHARGE;
	PWM_Configuration();
	DelayUsRaw (100000);
	SetDispLedBrightness( NOAUTO, 100, 100 );

	// Lademoeglichkeit pruefen und Laden einschalten
	CheckExtPower();
	SetExtPowerMode();
	StoppTime = 0;
	TIM_SetCompare1( TIM17, 100);
	I2C_Data.Bq24192.InpSrcContr = INP_CURR_1200MA;
	I2C_Data.Bq24192.PwrOnConf = 0x57;
	I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_4096MA;
	i=0; err = 1;
	while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
	SETSOURCE_EXT;
	SETCHARGE;
	DoneCount = 0;
	BatState.IntCalibState = CALIB_FULLCHARGE;

	// -------------- Ab hier: Laden und warten, bis Akku voll ist ----------------
	do
	{
		DelayUsRaw (1000000);
		CheckChargingMode();

		if ( (StoppTime%60) == 20)
		{
			DoSysAdMeasurements();
			BatState.IntCurrent = AnalogVals.Akku_mA_LT;
			BatState.IntVoltage = AnalogVals.Akku_mV_LT;
			BatState.IntCalibState = CALIB_FULLCHARGE;

#ifdef ACCUDEBUG
			DoneCount+= 2;
			DbgPrintf ("\r\n100P-Laden. U = %d, I = %d, CS = %d", BatState.IntVoltage, BatState.IntCurrent, BatState.IntCalibState);
#else
			SendTeleCh( TID_BATTERY_STATE, &BatState, sizeof(BatState), COM_CHANNEL_RS );
#endif

			DelayMs (50);
			while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
			DelayMs (50);
			SETSOURCE_EXT;
			SETCHARGE;
			SetDispLedBrightness( NOAUTO, 100, 100 );
		}

		if ( (BatState.IntChrgMode == CHRGMODE_DONE) ) DoneCount++;
		else if ( (AnalogVals.Akku_mA_LT > -900) && (AnalogVals.Akku_mA_LT < -500) && (AnalogVals.Akku_mV_LT > 4100) ) DoneCount++;
#ifndef ACCUDEBUG
		else DoneCount = 0;
#endif

		// LEDs in Gelb blinken lassen
		Tm4 = StoppTime % 4;
		if (Tm4 == 0) { TIM_SetCompare1( TIM1, 175 ); TIM_SetCompare2( TIM1, 225); TIM_SetCompare4( TIM1, 0 ); TIM_SetCompare1( TIM4, 0); TIM_SetCompare3( TIM4, 0); TIM_SetCompare4( TIM4, 0); }
		if ( (Tm4 == 1 ) || (Tm4 == 3) ) { TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare2( TIM1, 0); TIM_SetCompare4( TIM1, 175 ); TIM_SetCompare1( TIM4, 225); TIM_SetCompare3( TIM4, 0); TIM_SetCompare4( TIM4, 0); }
		if (Tm4 == 2) { TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare2( TIM1, 0); TIM_SetCompare4( TIM1, 0 ); TIM_SetCompare1( TIM4, 0); TIM_SetCompare3( TIM4, 175); TIM_SetCompare4( TIM4, 225); }
		TIM_SetCompare3( TIM1, 0); TIM_SetCompare2( TIM4, 0); TIM_SetCompare1( TIM3, 0);
		StoppTime++;
	} while ( (DoneCount < 6) && (StoppTime < 72000) );

	StoppTime = 0;
	TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare2( TIM1, 0); TIM_SetCompare4( TIM1, 0 );
	TIM_SetCompare1( TIM4, 0); TIM_SetCompare3( TIM4, 0); TIM_SetCompare4( TIM4, 0);
}

/******************************************************************************************************
                                   2) BM_DisCharging
-----------------------------------------------------------------------------------------------------*/
s8 BM_DisCharging (void)
{
	u8  DoneCount, SumNr, i, Tm4;
	u16 n = 0, loop = 0;
	u32 CurrSum, StartTime, StoppTime = 0, CalibSum, RtcTime;
	s32 TimeDiff;
	float TimeDiffFloat;
	s16 err = 0;

	// ------------- Ab hier: Entladen und warten, bis Akku leer ist ---------------
	// Laden abschalten. Anschliessend Stromverbrauch erhoehen und messen
	SW_3V3_ON;					// 3,3V-Versorgung einschalten
	SW_5V_ON;					// 5V-Versorgung einschalten
	HwModuleMask.OnOff = 0x031C;// HW-Modul-Schaltmaske definieren
	HwModuleMask.Error = 0;		// Fehlermaske loeschen
	ModuleSwitch();				// HW gemaess Maske einstellen
	I2C_Configuration ();
	DelayUsRaw (1000000);
	I2C_Init_IC (BQ24192);
	SETSOURCE_USB;
	SETNOCHARGE;
	DelayMs (100);

	FlashLedState.ModeMask = 0x0; SetFlashLedMode ();
	DelayUsRaw (1000000);
	I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
	I2C_Data.Bq24192.PwrOnConf = 0x07;
	I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
	i=0; err = 1;
	while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
	DelayMs (100);

	FlashLedState.BrightComp[0] = 600;
	FlashLedState.BrightComp[1] = 600;
	FlashLedState.ModeMask = 0;
	FlashLedState.OnTime = 0;
	FlashLedState.PeriodicTime = 0;
	SetFlashLedMode ();
	DelayUsRaw (100000);
	SetDispLedBrightness( NOAUTO, 200, 200 );
	StartTime = sys_tic;			// Startzeit speichern

	{
		// Array und Struktur aufgrund der Gr��e nur lokal definieren
		u16 CurrInt[180];
		u16 CurrTemp[24];
		BatState.IntCalibState = CALIB_DISCHARGE;

		// Waehrend des Entladens: regelmaessig Strom & Spannung messen
		do
		{
			// sechs Minuten lang jeweils alle 15 Sekunden Werte aufnehmen
			n = 0;
			DoneCount = 0;

			do
			{
				DelayUsRaw (1000000);
				if ( (StoppTime%15) == 1)
				{
					// Spannung und Strom (LongTime) messen
					DoSysAdMeasurements();
					CurrTemp[n] = AnalogVals.Akku_mA_LT;
					BatState.IntCurrent = CurrTemp[n];
					BatState.IntCalibState = CALIB_DISCHARGE;

					if ( (n % 4) == 0)
					{
						BatState.IntChrgMode = CHRGMODE_NOCHRG;

#ifdef ACCUDEBUG
						DoneCount += 1;
						DbgPrintf ("\r\nEntladen. n = %d, U = %d, I = %d, CS = %d", n, BatState.IntVoltage, BatState.IntCurrent, BatState.IntCalibState);
#else
						SendTeleCh( TID_BATTERY_STATE, &BatState, sizeof(BatState), COM_CHANNEL_RS );
#endif

						DelayMs (50);
						I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
						I2C_Data.Bq24192.PwrOnConf = 0x07;
						I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
						while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
						DelayMs (50);
						SETNOCHARGE;

						// hohen Strombedarf einstellen (LEDs an)
						DAC_SetChannel1Data(DAC_Align_12b_R, 600);
						TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_Active);
						DelayUsRaw (1000);
						DAC_SetChannel2Data(DAC_Align_12b_R, 600);
						TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_Active);
						DelayUsRaw (1000);
						SENSOR_ON;
						DelayUsRaw (1000);
						TIM_ForcedOC1Config(TIM17, TIM_ForcedAction_Active);
						SETSOURCE_USB;
						SETNOCHARGE;
						DelayUsRaw (1000);
						I2C_Data.Bq24192.InpSrcContr = INP_CURR_100MA;
						I2C_Data.Bq24192.PwrOnConf = 0x07;
						I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
						i=0; err = 1;
						while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
					}
					n++;
				}

				// LEDs in Pink blinken lassen
				Tm4 = StoppTime % 4;
				if (Tm4 == 0) { TIM_SetCompare1( TIM1, 600 ); TIM_SetCompare3( TIM1, 200); TIM_SetCompare4( TIM1, 0 ); TIM_SetCompare2( TIM4, 0); TIM_SetCompare3( TIM4, 0); TIM_SetCompare1( TIM3, 0); }
				if ( (Tm4 == 1) || (Tm4 == 3) ) { TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare3( TIM1, 0); TIM_SetCompare4( TIM1, 600 ); TIM_SetCompare2( TIM4, 200); TIM_SetCompare3( TIM4, 0); TIM_SetCompare1( TIM3, 0); }
				if (Tm4 == 2) { TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare3( TIM1, 0); TIM_SetCompare4( TIM1, 0 ); TIM_SetCompare2( TIM4, 0); TIM_SetCompare3( TIM4, 600); TIM_SetCompare1( TIM3, 200); }
				TIM_SetCompare2( TIM1, 0); TIM_SetCompare1( TIM4, 0); TIM_SetCompare4( TIM4, 0);
				StoppTime++;

				if ( (AnalogVals.Akku_mV_LT <= 3050) ) DoneCount++;
			} while ( ( DoneCount < 6 ) && ( n < 24 ) );

			// Berechnungen w�hrend des Entladens
			if (DoneCount < 6)
			{
				// Durchschnitt der gueltigen Werte bilden
				CurrSum = 0; SumNr = 0;
				for (n=0; n<24; n++)
				{
					if ( (CurrTemp[n] > 400) && (CurrTemp[n] < 5000) ) 
					{
						CurrSum += CurrTemp[n];
						SumNr++;
					}
				}
				CurrSum /= SumNr;
				SumNr = 0;

				CurrInt[loop] = CurrSum;

#ifdef ACCUDEBUG
				DoneCount += 2;
				DbgPrintf ("\r\nAkku wird entladen. Werte %d: CH: %d, CL: %d, VH: %d, VL: %d", loop, CurrHigh[loop], CurrLow[loop], VoltHigh[loop], VoltLow[loop]);
#endif

				loop++;
			}
		} while ( ( DoneCount < 6 ) && ( loop < 180 ) );	// Entladeschleifen-Ende

		StoppTime = sys_tic;			// Stoppzeit speichern

		// sparsamen Modus einstellen
		SW_3V3_ON;					// 3,3V-Versorgung einschalten
		SW_5V_ON;					// 5V-Versorgung einschalten
		HwModuleMask.OnOff = 0x031C;// HW-Modul-Schaltmaske definieren
		HwModuleMask.Error = 0;		// Fehlermaske loeschen
		ModuleSwitch();				// HW gemaess Maske einstellen
		SetDispLedBrightness( 0, 100, 100 );
		DelayUsRaw (1000000);

		// Lademoeglichkeit pruefen und Laden einschalten
		CheckExtPower();
		SetExtPowerMode();
		FlashLedState.ModeMask = 0x0; SetFlashLedMode ();
		TIM_SetCompare1( TIM17, 50);
		SETCHARGE;
		SETSOURCE_EXT;
		DelayUsRaw (100000);
		I2C_Data.Bq24192.InpSrcContr = INP_CURR_1500MA;
		I2C_Data.Bq24192.PwrOnConf = 0x57;
		I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_4096MA;
		i=0; err = 1;
		while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }

		// hier die Berechnungen durchf�hren (am Anfang des Ladevorgangs)
		TimeDiff = StoppTime - StartTime;
		if (TimeDiff < 0) TimeDiff = (4294967296 - StartTime) + StoppTime;
		TimeDiff /= 1000;

		// ggf. erste Werte korrigieren
		if ( (CurrInt[0] < 550) || (CurrInt[0] > 4000) ) CurrInt[0] = CurrInt[1] - 10;

		// Mittelung der Werte und evtl. Auffuellen der Luecken
		for (n=1; n<(loop-1); n++)
		{
			if ( (CurrInt[n] < 500) || (CurrInt[n] > 4000) )
				CurrInt[n] = (CurrInt[n-1] + CurrInt[n+1]) >> 1;

			if ( (n>1) && (n<(loop-2) ) )
			{
				if ( (CurrInt[n] < 500) || (CurrInt[n] > 4000) || (CurrInt[n] < (CurrInt[n-1]-5) ) || (CurrInt[n] > (CurrInt[n+1]+5) ) )
					CurrInt[n] = (CurrInt[n-2] + CurrInt[n+2]) >> 1;
			}
		}

		// ggf. letzte Werte korrigieren
		if ( (CurrInt[loop-1] < 500) || (CurrInt[loop-1] > 4000) || (CurrInt[loop-1] < ( CurrInt[loop-2]-5) ) )
			CurrInt[loop-1] = CurrInt[loop-2] + 10;

		// zweiter Durchlauf der Wertkorrekturen
		for (n=1; n<(loop-1); n++)
		{
			if ( (CurrInt[n] < 500) || (CurrInt[n] > 4000) || (CurrInt[n] < (CurrInt[n-1]-5) ) || (CurrInt[n] > (CurrInt[n+1]+5) ) )
				CurrInt[n] = (CurrInt[n-1] + CurrInt[n+1] ) >> 1;
		}

		if ( (CurrInt[loop-1] < 500) || (CurrInt[loop-1] > 4000) || (CurrInt[loop-1] < (CurrInt[loop-2]-5) ) )
			CurrInt[loop-1] = CurrInt[loop-2] + 10;

		// Berechnung der Akkukapazitaet mit Hilfe der Einzelwerte
		CalibSum = 0;
		for (n=0; n<loop; n++) CalibSum += CurrInt[n];

		{
			BatDataStrct BatteryData;
			CalibSum = (u32)((float)(CalibSum / loop) * (float)TimeDiff / 3600.0);
			BatteryData.mfgmagic1 = 0x01020304;
			BatteryData.mfgmagic2 = 0x04030201;
			RTC_GetTime( &RtcTime );
			if (RtcTime < 1262300400) RtcTime = 1262300400; // korrigieren, falls < 1.1.2010
			BatteryData.calib_unix_time = RtcTime;
			BatteryData.time_dummy = 0;

			if ((CalibSum > 10000) && (CalibSum < 30000))
			{
				BatState.IntCalibState = CALIB_DONE;
				BatState.IntCalibCapacity = (u16)CalibSum;
				BatteryData.calib_capacity = (u16)CalibSum;
			}
			else
			{
				BatState.IntCalibState = CALIB_NOK;
				BatState.IntCalibCapacity = 0;
				BatteryData.calib_capacity = 0;
			}			
				
			for (i=0; i<100; i++)
			{ BatteryData.res_array[i] = 0; BatteryData.voltage_array[i] = 0; }
			BatteryData.res_array[0] = BatData->res_array[0];
			if (EraseFlash( (void*)BatData, (void *)BatData + 512) != 0) return -3;
			if (WriteFlash( &BatteryData, (void *)BatData, 512) != 0) return -4;
		}
	
		CalibSum = 0;
		TimeDiffFloat = (float)loop / 102.0;
	} 
	return 0;
}

/******************************************************************************************************
                                   3) BM_PartCharging
------------------------------------------------------------------------------------------------------*/
void BM_PartCharging ( u8 endstate )
{
	s16 err = 0;
	u8 i = 0;

	// -------- Ab hier: wieder Laden und warten, bis Akku teilweise geladen ist ----------
	//StoppTime = 0; Value = 0;
	PWM_Configuration();
	SetDispLedBrightness( NOAUTO, 350, 350 );
	CheckExtPower();
	SetExtPowerMode();
	I2C_Configuration ();
	DelayUsRaw (100000);
	I2C_Data.Bq24192.InpSrcContr = INP_CURR_1200MA;
	I2C_Data.Bq24192.PwrOnConf = 0x57;
	I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_4096MA;
	err = 1;
	while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
	SETSOURCE_EXT;
	SETCHARGE;
	FlashLedState.ModeMask = 0x0; SetFlashLedMode ();
}
