/**********  #INCLUDES            **********/
#include "system.h"
#include "syslib.h"
#include "globals.h"
#include "sage.h"
#include "serial.h"
#include "task.h"
#include "irq.h"
#include "power.h"
#include "I2C.h"

#define USHRT_MAX 1800000

/**********   #DEFINES             **********/
/**********   ENUMS                **********/

/**********   STRUCTS              **********/
/**********   CONSTS               **********/
/**********   Public Variables     **********/
extern u16 NoChangeCount;
extern u8 LedTestFlag;

// AD-Wandler-Schwellwerte fuer die Akkutemperaturen. Berechnung: (Wert aus Datenblatt
// B57861S0103F040) / (Wert + 1) * 4096. Datenblatttabelle auf 1K bezogen, Spannungsteiler
// mit 10K. Die Berechnung der Zwischenwerte erfolgt über einfache lineare Interpolation
const u16 NtcAdAkku[22] =
{
//	-25°C	-20°C	-15°C	-10°C	-5°C	0°C		5°C		10°C	15°C	20°C	25°C
    3804,   3713,   3602,   3469,   3313,   3136,   2939,   2726,   2503,   2275,   2048,
//	30°C	35°C	40°C	45°C	50°C	55°C	60°C	65°C	70°C	75°C	80°C
    1828,   1618,   1424,   1245,   1085,   942,    816,    706,    611,    528,    458
};

// AD-Wandler-Schwellwerte fuer die Kühlkörpertemperatur. Berechnung: (Wert aus Datenblatt
// NCP18XH103F03RB) / (Wert + 10) * 4096. Datenblatttabelle auf 10K bezogen, Spannungsteiler
// mit 10K. Die Berechnung der Zwischenwerte erfolgt über lineare Interpolation
const u16 NtcAdKuehler[22] =
{
//	-25°C	-20°C	-15°C	-10°C	-5°C	0°C		5°C		10°C	15°C	20°C	25°C
	3676,	3572,	3452,	3316,	3163,	2995,	2817,	2629,	2436,	2241,	2048,
//	30°C	35°C	40°C	45°C	50°C	55°C	60°C	65°C	70°C	75°C	80°C
	1860,	1679,	1509,	1350,	1204,	1070,	949,	842,	746,	661,	586
};

/**********   Public Functions     **********/

/**********   Private Variables    **********/
//static u16 fPowerDown;            // Spannungsversorgung kritisch . Ausschalten

/**********   Private Functions    **********/
u8   AdcConn2Nr( u16 AdcConn );
void DoSysAdMeasurements( void );
//u8   SetBattState( void );

/**********   Code                 **********/

/******************************************************************************************************
                                            InitSystemAgent
-------------------------------------------------------------------------------------------------------
Initialisiert den sage-Task
******************************************************************************************************/
void SageTsk_Init( void ) 
{
	StopTask( SAGE_TASK );
}

/******************************************************************************************************
                                        SageTsk_CheckForAction
-------------------------------------------------------------------------------------------------------
Wird vom SysTick-Irq alle 10ms aufgerufen
******************************************************************************************************/
void SageTsk_CheckForAction( void ) 
{
	static u8 ops = POWER_UNKNOWN, first = 1;

	if (PowerState != POWEROFF_NOTCHARGING_LOOP)
	{
		CheckLedState();							// LED blinken lassen
		CheckForCommTimeout();						// RS232-Empfangs-Timeout
		KeyTest();									// Tastendruck und Betriebszustand ueberpruefen

		if (RgbLedState.GeneralBrightness == 1001)
		{
			if (DispBacklight.Mode == 1)			// Display- und LED-Helligkeit automatisch regeln
			{ SetDispLedBrightness( AUTOLEDSDISP, 0, 0 ); }
			else										// nur LED-Helligkeit automatisch regeln
			{ SetDispLedBrightness( AUTOLEDS, DispBacklight.Brightness, 0 ); }
		}
		else
			if (DispBacklight.Mode == 1)			// Display-Helligkeit automatisch regeln
			{ SetDispLedBrightness( AUTODISP, 0, RgbLedState.GeneralBrightness ); }
			else										// nichts automatisch regeln
			{ SetDispLedBrightness( NOAUTO, DispBacklight.Brightness, RgbLedState.GeneralBrightness ); }
	}

	// alle 400 mSec (im abgeschalteten Modus: immer) den aktuellen Betriebszustand bestimmen
	if ( ( (sys_tic % 400) == 0 ) || (PowerState == POWEROFF_NOTCHARGING_LOOP) )
	{
		PowerState = GetPowerMode();

		switch (PowerState)
		{
			case POWEROFF_NOTCHARGING_INIT:
				DbgPulse (1);
				ChargerType = CHRGMODE_NOCHRG;
				SetExtPowerMode();
				DelayUsRaw (30000);
				OldChargingState = CHRG_NODETECT;
				SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
				PowerOff ();
				PowerState = POWEROFF_NOTCHARGING_LOOP;
			break;

			case POWEROFF_NOTCHARGING_LOOP:
				//DbgPulse (1);
			break;

			case POWEROFF_CHARGING_INIT:
			{
				// Hardware für den Ladezustand richtig initialisieren
				PowerOffCharging ();
				I2C_Configuration ();
				DbgPulse (2);
				DelayUsRaw (20000);
				I2C_Init_IC (BQ24192);
				PowerState = POWEROFF_CHARGING_INIT2;
			}
			break;

			case POWEROFF_CHARGING_INIT2:
			{
				u8 i=0, ChrgMode1, ChrgMode2;

				do
				{
					CheckExtPower();
					ChrgMode1 = ChargerType;
					DelayUsRaw (40000);
					CheckExtPower();
					ChrgMode2 = ChargerType;
					i++;
				} while ( ( ChrgMode1 != ChrgMode2) && (i<8) );

				SetExtPowerMode();
				PowerState = POWEROFF_CHARGING_LOOP;
			}
			break;

			case POWEROFF_CHARGING_LOOP:
			{
				static u8 i=0, fr=1;

				CheckChargingMode();
				DoSysAdMeasurements ();

				if (i%4 == 1)
				{
					int j;
					u32 MaxOff = 0;
					for (j=0; j<12; j++) if (RgbLedState.FinalOffTime[j] > MaxOff) MaxOff = RgbLedState.FinalOffTime[j];
					if ( CheckExtPower() ) { PowerState = POWEROFF_CHARGING_INIT2; }
					if ( ( (AnalogVals.Akku_mA > -200) && ( (I2C_Data.Bq24192.SysStatus & 0x30) != 0x30)
					    && (ChargerType < 0x20) ) || ( (fr==1) && (MaxOff < 10000) ) )
					{ PowerState = POWEROFF_CHARGING_INIT; } 
					
					fr = 0; 
				}

				i++;
			}
			break;

			case POWERON_NOTCHARGING_INIT:
				// Hardware für den eingeschalteten Zustand initialisieren
				if ( (ops == POWER_UNKNOWN) || (ops == POWEROFF_NOTCHARGING_LOOP) )
				{
					PowerOn ();
					Beep( 4000, 200 );
					SetLedState( LED_RGB4_WHITE, 250, 250, 250, 30000, 500, 500 );
				}

				// Laden beim naechsten Anstecken eines Netzteils aktivieren
				DelayUsRaw (30000);
				I2C_Configuration ();
				I2C_Init_IC (BQ24192);
				ChargerType = CHRGMODE_NOCHRG;
				SetExtPowerMode();
				CheckChargingMode();
				
				// Naechster Zustand: On-Loop
				DbgPulse (3);
				PowerState = POWERON_NOTCHARGING_LOOP;
			break;

			case POWERON_NOTCHARGING_LOOP:
			{
				static u8 i=0;

				if (LedTestFlag) LedTest();
				
				if ( (i%4) == 0)
				{
					OvercurrentTest ();
					DoSysAdMeasurements ();
					GetBatLevel ();
				}

				if ( (i%4) == 1)
				{
					if (BatState.IntChrgState < ABS_EMPTY) 
					{ 
						SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE, 0, 0, 0, 0, 0, 0 ); 
						SetLedState( LED_RGB1_RED, 800, 0, 0, 1000, 50, 300 ); 
					}
					else if (BatState.IntChrgState < EMPTY) { SetLedState( LED_RGB1_RED, 300, 0, 0, 1250, 1250, 0 ); }
					else if (BatState.IntChrgState < LOWMED) { SetLedState( LED_RGB1_YELLOW, 25, 175, 0, 2000, 2000, 0 ); }
					else if (BatState.IntChrgState < MEDHIGH) { SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW, 25, 175, 0, 2000, 2000, 0 ); }
					else { SetLedState( LED_RGB1_YELLOW | LED_RGB2_YELLOW | LED_RGB3_YELLOW, 25, 175, 0, 2000, 2000, 0 ); }
					//DbgPrintf ("Brightness: %d", AnalogVals.Ambient_AD);

					if (first == 1)
					{
						HwModuleMask.OnOff |= 0x0004;
						ModuleSwitch();
					}
				}

				if ( (i == 18) && (first == 1) )
				{
					DispBacklight.Mode = 1;
					SetDispLedBrightness( AUTOLEDSDISP, 0, 0 );
					first = 0;
				}
				i++;
			}
			break;

			case POWERON_CHARGING_INIT:

				// Hardware für den Ladezustand richtig initialisieren
				if ( (ops == POWER_UNKNOWN) || (ops == POWEROFF_NOTCHARGING_LOOP) )
				{
					PowerOnCharging ();
					Beep( 4000, 200 );
					SetLedState( LED_RGB4_WHITE, 250, 250, 250, 30000, 500, 500 );
					DelayUsRaw (30000);
					I2C_Configuration ();
					I2C_Init_IC (BQ24192);
				}
				else
				{
					DelayUsRaw (30000);
					I2C_Configuration ();
					I2C_Init_IC (BQ24192);
				}

				DbgPulse (4);
				PowerState = POWERON_CHARGING_INIT2;
			break;

			case POWERON_CHARGING_INIT2:
			{
				static u8 i=0;
				u8 ChrgMode1, ChrgMode2;
				do
				{
					CheckExtPower();
					ChrgMode1 = ChargerType;
					DelayUsRaw (40000);
					CheckExtPower();
					ChrgMode2 = ChargerType;
					i++;
				} while ( ( ChrgMode1 != ChrgMode2) && (i<8) );
				SetExtPowerMode();
				PowerState = POWERON_CHARGING_LOOP;
			}
			break;

			case POWERON_CHARGING_LOOP:
			{
				static u8 i=0, fr=1;

				CheckChargingMode();
				DoSysAdMeasurements ();

				if (i%4 == 1)
				{
					int j;
					u32 MaxOff = 0;
					for (j=0; j<12; j++) if (RgbLedState.FinalOffTime[j] > MaxOff) MaxOff = RgbLedState.FinalOffTime[j];
					GetBatLevel ();
					if ( CheckExtPower() ) { PowerState = POWERON_CHARGING_INIT2; }
					if ( ( (AnalogVals.Akku_mA > 0) && ( (I2C_Data.Bq24192.SysStatus & 0x30) != 0x30)
					    && (ChargerType < 0x20) ) ||  ( (fr==1) && (MaxOff < 10000) ) ) 
					{ PowerState = POWERON_CHARGING_INIT; }
					OvercurrentTest();
					fr=0; 

					if (first == 1)
					{
						HwModuleMask.OnOff |= 0x0004;
						ModuleSwitch();
					}
				}

				if ( (i == 18) && (first == 1) ) 
				{
					DispBacklight.Mode = 1;
					SetDispLedBrightness( AUTOLEDSDISP, 0, 0 );
					first = 0;
				}

				i++;
			}
			break;
		}
		ops = PowerState;
	}

    //DbgPulse (1);
	StopTask( SAGE_TASK );
}

/******************************************************************************************************
                                        DoSysVoltMeasurements
-------------------------------------------------------------------------------------------------------
Wertet die ADC1-Messung der Akkuspannung, des Akkustroms
******************************************************************************************************/
void DoSysAdMeasurements( void )
{
	u8 i;
	u16 AV = 0, AVL = 0, KT = 0, Amb = 0;
	u16 IUV = 0, I1V = 0, I2V = 0, IC = 0;
	u16 AI = 0, AIL = 0;
	float TempDiff;

	// Summenwerte bilden
	for (i=0; i<16; i++)
	{
		Amb += AmbVolt[i]; AV += AkkVolt[i];
		AI += AkkCurr[i];  KT += KuehlTemp[i];
		AVL += AkkVoltLt[i]; AIL += AkkCurrLt[i];
	}
	for (i=0; i<4; i++)
	{
		IUV += UsbVolt[i]; I1V += In1Volt[i];
		I2V += In2Volt[i]; IC += InpCurr[i];
	}

	// Umrechnung von A/D-Stufen (16 Werte) auf Millivolt (Akkuspannung)
	AnalogVals.Akku_mV = (u16) ( (float)AV * 0.07631258);		// 5000mV / 4095 Stufen / 16 Werte
	AnalogVals.Akku_mV_LT = (u16) ( (float)AVL * 0.07631258);

	// Umrechnung von A/D-Stufen (16 Werte) auf Milliampere (Akkustrom)
	AnalogVals.Akku_mA = (s16) ( (float)AI * 0.15262515) - 5000;// 10000mA / 4095 Stufen / 16 Werte
	AnalogVals.Akku_mA_LT = (s16) ( (float)AIL * 0.15262515) - 5000;

	// Umrechnung von A/D-Stufen (16 Werte) auf Temperatur (Kühlkörpertemperatur)
	KT >>= 4; i = 0; while ( (KT < NtcAdKuehler[i]) && (i<22) ) i++;
	if (i>0)
	{
		TempDiff = 5.0 * ( ((float)KT - (float)NtcAdKuehler[i]) / ((float)NtcAdKuehler[i-1] - (float)NtcAdKuehler[i]) );
		AnalogVals.TempKuehlDeg = (i*5) - 25 - ( (u16) (TempDiff + 0.5) );
	}
	else { AnalogVals.TempKuehlDeg = -26; }

	// Helligkeitswert direkt übernehmen (0...65536)
	AnalogVals.Ambient_AD = Amb;
	
	// Umrechnung von A/D-Stufen (4 Werte) auf Millivolt (USB-Eingang)
	AnalogVals.Usb_mV = (s16) ( (float)IUV * 0.61512562 );		// 10075,7575mV / 4095 Stufen / 4 Werte

	// Umrechnung von A/D-Stufen (4 Werte) auf Millivolt (Spannungeingang 1)
	AnalogVals.In1_mV = (s16) ( (float)I1V * 1.47980039 );		// 24239,1304mV / 4095 Stufen / 4 Werte

	// Umrechnung von A/D-Stufen (4 Werte) auf Millivolt (Spannungeingang 2)
	AnalogVals.In2_mV = (s16) ( (float)I2V * 1.47980039 );		// 24239,1304mV / 4095 Stufen / 4 Werte

	// Umrechnung von A/D-Stufen (4 Werte) auf Milliampere (Eingangsstrom)
	AnalogVals.InCurr_mA = (s16) ( (float)IC * 0.674603 );		// 4420 mA / 1638 Stufen / 4 Werte

	// Umrechnung von A/D-Stufen (1 Wert) auf Temperatur (Akkutemperatur 1)
	i = 0; while ( (AnalogVals.TempAkku1Raw < NtcAdAkku[i]) && (i<22) ) i++;
	if (i>0)
	{
		TempDiff = 5.0 * ( ((float)AnalogVals.TempAkku1Raw - (float)NtcAdAkku[i]) / ((float) NtcAdAkku[i-1] - (float)NtcAdAkku[i]) );
		AnalogVals.TempAkku1Deg = (i*5) - 25 - ( (u16) (TempDiff + 0.5) );
	}
	else { AnalogVals.TempAkku1Deg = -26; }

	// Umrechnung von A/D-Stufen (1 Wert) auf Temperatur (Akkutemperatur 2)
	i = 0; while ( (AnalogVals.TempAkku2Raw < NtcAdAkku[i]) && (i<22) ) i++;
	if (i>0)
	{
		TempDiff = 5.0 * ( ((float)AnalogVals.TempAkku2Raw - (float)NtcAdAkku[i]) / ((float) NtcAdAkku[i-1] - (float)NtcAdAkku[i]) );
		AnalogVals.TempAkku2Deg = (i*5) - 25 - ( (u16) (TempDiff + 0.5) );
	}
	else { AnalogVals.TempAkku2Deg = -26; }

	//DbgPrintf ("AV = %d mV / AI = %d mA / Hell = %d / UV = %d mV / U1In = %d mV / U2In = %d mV / IIn = %d mA / ST = %d °C",
	//		    AnalogVals.Akku_mV, AnalogVals.Akku_mA, AnalogVals.Ambient_AD, AnalogVals.Usb_mV, AnalogVals.In1_mV, AnalogVals.In2_mV, AnalogVals.InCurr_mA, AnalogVals.Temp_STM/10 );
}

/******************************************************************************************************
                                        LedTest
-------------------------------------------------------------------------------------------------------
Erzeugt ein LED Lauflicht von 'sage.c' aus
******************************************************************************************************/

void LedTest (void)
{
	static u8 i = 0;

	switch (i)
	{
		case 0:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB1_RED, 500, 0, 0, 1000, 1000, 0 );
		break;

		case 1:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB2_RED, 500, 0, 0, 1000, 1000, 0 );
		break;

		case 2:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB3_RED, 500, 0, 0, 1000, 1000, 0 );
		break;

		case 3:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB4_RED, 500, 500, 0, 1000, 1000, 0 );
		break;

		case 4:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB1_GREEN, 0, 500, 0, 1000, 1000, 0 );
		break;

		case 5:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB2_GREEN, 0, 500, 0, 1000, 1000, 0 );
		break;

		case 6:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB3_GREEN, 0, 500, 0, 1000, 1000, 0 );
		break;

		case 7:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB4_GREEN, 0, 500, 0, 1000, 1000, 0 );
		break;

		case 8:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB1_BLUE, 0, 0, 500, 1000, 1000, 0 );
		break;

		case 9:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB2_BLUE, 0, 0, 500, 1000, 1000, 0 );
		break;

		case 10:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB3_BLUE, 0, 0, 500, 1000, 1000, 0 );
		break;

		case 11:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB4_BLUE, 0, 0, 500, 1000, 1000, 0 );
		break;

		case 12:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB1_WHITE, 250, 250, 250, 1000, 1000, 0 );
		break;

		case 13:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB2_WHITE, 250, 250, 250, 1000, 1000, 0 );
		break;

		case 14:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB3_WHITE, 250, 250, 250, 1000, 1000, 0 );
		break;

		case 15:
			SetLedState( LED_RGB1_WHITE | LED_RGB2_WHITE | LED_RGB3_WHITE | LED_RGB4_WHITE, 0, 0, 0, 0, 0, 0 );
			SetLedState( LED_RGB4_WHITE, 250, 250, 250, 1000, 1000, 0 );
		break;
	}

	if (i<15) i++;
	else i=0;
}
