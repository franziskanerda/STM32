/** Description        *******************************************************************************/

/** #INCLUDES          *******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "globals.h"
#include "syslib.h"
#include "serial.h"
#include "comm.h"
#include "crc.h"
#include "teles.h"
#include "info.h"
#include "power.h"
#include "gpio_map.h"
#include "sage.h"
#include "flash.h"
#include "i2c.h"

/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/
extern BatStateStruct BatState;
ExtPwrStateStruct ExtPwrState;
/** Public Functions   *******************************************************************************/
/** Private Variables  *******************************************************************************/

/** Private Functions  *******************************************************************************/
void DoTeleResend( RxBufStrct *rxb );
void DoTeleGetLastError( RxBufStrct *rxb );
void DoTeleEventData( RxBufStrct *rxb );
void DoTeleHwModulesOnOff( RxBufStrct *rxb );
void DoTeleDeviceInfo( RxBufStrct *rxb );
void DoTeleGetPowerState( RxBufStrct *rxb );
void DoTeleRgbLedState( RxBufStrct *rxb );
void DoTeleFlashLedState( RxBufStrct *rxb );
void DoTeleDispBrightness( RxBufStrct *rxb );
void DoTeleRtc( RxBufStrct *rxb );
void DoTeleBeep( RxBufStrct *rxb );
void DoTeleGetCrc( RxBufStrct *rxb );
void DoTeleStartBatCal( RxBufStrct *rxb );
void DoTeleHwProdInfo( RxBufStrct *rxb );
void DoTeleGetTemperature( RxBufStrct *rxb );
void DoTeleGetDebugData( RxBufStrct *rxb );


/** Code               *******************************************************************************/

/******************************************************************************************************
                                                DoTele
-------------------------------------------------------------------------------------------------------
Wird vom Rx-Task aufgerufen wenn ein vollständiges Telegramm angekommen ist.
Wenn eine Telegramm angekommen ist stehen in rxb->rdptr die empfangenen Daten, rxb->cnt enthält die
Anzahl der empfangenen Daten in Byte. In rxb->tid steht die empfangene Telegrammnummer, rxb->channel
enthält den Kanal auf dem empfangen wurde.

Parameter:
   rxb:      Zeiger auf Empfangsbuffer
******************************************************************************************************/
void DoTele( RxBufStrct *rxb )
{
	switch ( rxb->tid )
	{
		// hier starten die Standard-Telegramme
		case TID_RESEND_LAST_TELE:			// Fehlerfall: Daten nochmal schicken
			DoTeleResend( rxb );
		break;
	
		case TID_GET_LAST_ERROR:			// fragt den letzten Fehlerstatus ab
    		DoTeleGetLastError( rxb );
    	break;

    	// hier gibt's ein Event-Protokoll
		case TID_GET_EVENT:
			DoTeleEventData( rxb );
		break;

    	// hier starten die RunF-speziefischen Protokolle
		case TID_SET_HW_MODULES_ONOFF:		// Schaltet verwendbare Module an und aus
		case TID_GET_HW_MODULES_ONOFF:		// liest, welche Module an- und ausgeschaltet sind
    		DoTeleHwModulesOnOff( rxb );
    	break;
	
    	case TID_GET_HW_INFO:				// Geraeteinfo lesen
        case TID_GET_FW_INFO:
    		DoTeleDeviceInfo( rxb );
    	break;

		case TID_GET_BATTERY_STATE:			// liest den aktuellen Status des / der Akkus
		case TID_GET_EXTPOWER_STATE:		// liest den Status der externen Stromversorgung
    		DoTeleGetPowerState( rxb );
    	break;

		case TID_SET_RGBLED_STATE:			// schaltet die RGB-LED-Komponenten ein- und aus
		case TID_GET_RGBLED_STATE:			// gibt den Status sämtlicher RGB-LED-Komponenten zurück
    		DoTeleRgbLedState( rxb );
    	break;

		case TID_SET_FLASHLED_STATE:		// schaltet rueckseitige weisse LEDs auf definierten Modus
		case TID_GET_FLASHLED_STATE:		// gibt den Status der rueckseitigen weissen LEDs zurück
    		DoTeleFlashLedState( rxb );
    	break;


		case TID_SET_DISP_BRIGHTNESS:		// stellt die Helligkeit der Displaybeleuchtung eins
		case TID_GET_DISP_BRIGHTNESS:		// liest die Helligkeitswerte der Displaybeleuchtung
    		DoTeleDispBrightness( rxb );
    	break;
		
		case TID_SET_RTC:					// stellt die RTC ein
		case TID_GET_RTC:					// liest die RTC-Daten zurück
    		DoTeleRtc( rxb );
    	break;
		
		case TID_BEEP:						// loest Ton im Piezo aus
			DoTeleBeep( rxb );
		break;
		
    	// Hier starten die Telegramme für die Produktion und Entwicklung
    	case TID_PROD_GET_FW_CRC:			// berechnet CRC-Summe der Applikation
    		DoTeleGetCrc( rxb );
    	break;
		
    	case TID_PROD_START_BATCALIB:    	// Start der Akkukalibrierung
    		DoTeleStartBatCal( rxb );
    	break;		

    	case TID_PROD_PROG_HWINFO:			// setzt die Masken fuer verfuegbare bzw. verwendbare HW-Module
		case TID_PROD_GET_HW_MODULMASK:		// liest die Masken fuer verfuegbare bzw. verwendbare HW-Module
			DoTeleHwProdInfo ( rxb );
    	break;

		case TID_GET_TEMPERATURE:
			DoTeleGetTemperature ( rxb );
		break;

		case TID_DEBUG_GET_DIGITAL_IOS:		// liest digitale IO-Werte aus dem STM32
		case TID_DEBUG_GET_ANALOG_VALS:		// liest analoge Werte aus den A/D-Wandlereingaengen
		case TID_DEBUG_GET_MEMORY_AREA:		// liest Speicherbereich aus
    		DoTeleGetDebugData( rxb );
		break;

		case TID_PROD_RESET:				// Resetten des STM32 laeuft momentan ausschliesslich per IO (Hardware)
    	break;

    	default:							// falsches Protokoll: Fehler
    		SendTeleNack( TERR_UNKNOWN_TELE, 0, rxb );
    	break;
	}
	ResetRxBuffer( rxb->channel );
}


/******************************************************************************************************
                                           DoTeleHwModulesOnOff
Schaltet einzelne Hardwarekomponenten bei Bedarf ein oder aus. Die Hardwarekomponenten sind folgender-
massen in der Variable 'modulemask' maskiert:
- Bit 0: 5,4V fuer Sensor	- Bit 1: Display/Backlight	- Bit 2: 2,8V fuer Kamera	- Bit 3: Touchcontroller
- Bit 4: SD-Karte			- Bit 5: Bluetooth-Modul	- Bit 6: WLAN-Modul			- Bit 7: NFC-Modul
- Bit 8: MEMS-IC			- Bit 9 - 15: ungenutzt
******************************************************************************************************/
void DoTeleHwModulesOnOff( RxBufStrct *rxb )
{
	if (rxb->tid == TID_SET_HW_MODULES_ONOFF)
	{
		u16 OldState = HwModuleMask.OnOff;
		u16 *hw_mask;
		u16 mask;

		// Hardwaremaske kommt per RS232, uebrige Masken stehen im Flash
		hw_mask = (u16 *)rxb->rdptr;
		HwModuleMask.Error = 0;
		HwModuleMask.OnOff = *hw_mask;
		HwModuleMask.Available = MfgData->available_mask;
		if (HwModuleMask.Available == 0xFFFF) HwModuleMask.Available = 0x067F;
		HwModuleMask.Usable = MfgData->activated_mask;
		if (HwModuleMask.Usable == 0xFFFF) HwModuleMask.Usable = 0x067F;

		// Maskendurchlauf
		for (mask = 0x0001; mask <= 0x0200; mask <<= 1)
		{
			switch (mask)
			{
				// 0x0001: Sensor (ohne Ladefunktion, immer verfuegbar)
				case 0x0001: if (HwModuleMask.OnOff & mask) { SENSOR_ON;    } else { SENSOR_OFF; 	} break;

				// 0x0002: Kamera (falls verfuegbar, muss zunaechst resettet werden)
				case 0x0002:
					if ( (HwModuleMask.OnOff & mask)
						&& (HwModuleMask.Available & 0x01)
						&& (HwModuleMask.Usable & 0x01) )
					{
						CAMERA_ON;
						if ( (OldState & 0x0002 ) == 0) CAM_RST;
					}
					else
					{
						CAMERA_OFF; CAM_RST;
						if (HwModuleMask.OnOff & mask) 	// Camera not available / usable
						{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; }
					}
				break;

				// 0x0004: Display Logik & Beleuchtung (immer verfuegbar)
				case 0x0004: if (HwModuleMask.OnOff & mask) { DISPLAY_ON;   } else { DISPLAY_OFF;   } break;

				// 0x0008: Touchscreen (immer verfuegbar)
				case 0x0008: if (HwModuleMask.OnOff & mask) { TOUCH_ON;     } else { TOUCH_OFF;     } break;

				// 0x0010: Micro-SD-Karte (immer verfuegbar)
				case 0x0010: if ( (HwModuleMask.OnOff & mask) ) { MICRO_SD_ON; } else { MICRO_SD_OFF; } break;

				// 0x0020: Bluetoothmodul (falls verfuegbar)
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

				// 0x0040: Bluetoothmodul (falls verfuegbar)
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

				// 0x0080 NFC-Modul (falls verfuegbar)
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

				// 0x0100: MEMs-Modul (falls verfuegbar)
				case 0x0100:
					if ( (HwModuleMask.OnOff & mask)
						&& (HwModuleMask.Available & 0x20)
						&& (HwModuleMask.Usable & 0x20) )
					{ MEMS_ON; }
					else
					{
						MEMS_OFF;
						if (HwModuleMask.OnOff & mask) 	// MEMs not available / usable
						{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; } 					}
				break;

				// LED-Beleuchtungsmodul (falls verfuegbar)
				case 0x0200:
					if ( (HwModuleMask.OnOff & mask)
						&& (HwModuleMask.Available & 0x01)
						&& (HwModuleMask.Usable & 0x01) )
					{ WLED_ON; }
					else
					{
						WLED_OFF;
						if (HwModuleMask.OnOff & mask) 	// WLED not available / usable
						{ HwModuleMask.Error |= mask; HwModuleMask.OnOff &= ~mask; } 					}
				break;
			}
		}

		// Masken mit geschalteten Ausgängen und evtl. Fehlern zurueckgeben
		SendTele( TID_MODULES_ONOFF, &HwModuleMask.OnOff, 2*sizeof(u16), rxb );
		if (HwModuleMask.OnOff & 0x0002) { DelayUsRaw (1000); CAM_NRST; }
		if (HwModuleMask.OnOff & 0x0200) I2C_Init_IC (LM75);
	}
	else
	{ SendTele( TID_MODULES_ONOFF, &HwModuleMask.OnOff, 2*sizeof(u16), rxb ); }
}


/******************************************************************************************************
                                           DoTeleEventData
Liefert die Daten des ausloesenden Ereignisses zurück. Diese Daten sollten vom iMX6 abgerufen werden,
wenn ein Event (STM <-> iMX6-Bit) ausgeloest wurde.
******************************************************************************************************/
void DoTeleEventData( RxBufStrct *rxb )
{
	s8 err = 0;

	// Struktur der zurückzuschickenden Daten
#pragma pack(2)
	struct
	{
		u16 triggerevent;
		u16 oc_mask;
		u16 extpwr_device;
		u16 batcharger_state;
		u16 charginglevel;
		s16 heatsink_temp;
		s16 battery1_temp;
		s16 battery2_temp;
		s16 ledboard_temp;
	} EvDataStruct;
#pragma pack()

	memset( &EvDataStruct, 0, sizeof(EvDataStruct) );

	// ausloesendes Event definieren
	EvDataStruct.oc_mask = OcEvent;
	OcEvent = 0;
	EvDataStruct.triggerevent |= GlobalEvent;
	GlobalEvent = 0;
	if (ChargerType == CHRG_DETERROR) EvDataStruct.triggerevent |= 0x0002;

	// Temperaturfehler als ausloesendes Exent (Akkutemperatur < -10°C | > 60°C
	if (AnalogVals.TempAkku1Deg < -100) EvDataStruct.triggerevent |= 0x0008;
	if (AnalogVals.TempAkku1Deg > 600) EvDataStruct.triggerevent |= 0x0010;
	if (AnalogVals.TempAkku2Deg < -100) EvDataStruct.triggerevent |= 0x0020;
	if (AnalogVals.TempAkku2Deg > 600) EvDataStruct.triggerevent |= 0x0040;

	// Werte fuellen
	EvDataStruct.extpwr_device = ChargerType;
	EvDataStruct.batcharger_state = BatState.IntChrgMode;
	EvDataStruct.charginglevel = BatState.IntChrgState;
	EvDataStruct.heatsink_temp = AnalogVals.TempKuehlDeg;

	err = I2C_Read (LM75, LM_RD_TEMP, 2);
	if (err==0) EvDataStruct.ledboard_temp = I2C_Data.Lm75.Temperature&0x00FF; // Daten kommen vertauscht an!
	else EvDataStruct.ledboard_temp = -1000;
	if (EvDataStruct.ledboard_temp & 0x0100) EvDataStruct.ledboard_temp ^= -EvDataStruct.ledboard_temp;

	if (AnalogVals.TempAkku1Raw > 4000) { EvDataStruct.battery1_temp = -1000; }
	else { EvDataStruct.battery1_temp = AnalogVals.TempAkku1Deg; }
	if (AnalogVals.TempAkku2Raw > 4000) { EvDataStruct.battery2_temp = -1000; }
	else { EvDataStruct.battery2_temp = AnalogVals.TempAkku2Deg; }

	// Telegramm ausgeben und Eventausgang loeschen
	SendTele( TID_EVENT, &EvDataStruct, sizeof(EvDataStruct), rxb );
	IMX6_STM1_OFF;
}


/******************************************************************************************************
                                           DoTeleDeviceInfo
holt sich die Geraeteinfo (Hardware bzw. Firmware) aus dem Flash bzw. aus sonstigen Quellen
******************************************************************************************************/
void DoTeleDeviceInfo( RxBufStrct *rxb )
{
	// Struktur der Firmware-Infodaten
	struct
	{
		u16 Version;
		u16 Upd_Date;
		u16 Upd_Year;
		u16 Upd_Time;
		u32 Rx_Size;
		u32 Tx_Size;
		u8  reserved1[32 - 2*sizeof(u32) - 4*sizeof(u16)];
	} FwInfoData;
	memset( &FwInfoData, 0, sizeof(FwInfoData) );

	char dat[12], tim[12], zs[12];
	u16 i;

	if ( rxb->tid == TID_GET_HW_INFO )
	{
		// Abfrage der Hardwareinfo (aus dem Flash)
		rxb->tid = TID_HW_INFO;
		HwInfoData.serno = MfgData->serno;
		if (HwInfoData.serno == 0xFFFFFFFF) HwInfoData.serno = 0;
		HwInfoData.date = MfgData->prod_date;
		if (HwInfoData.date == 0xFFFF) HwInfoData.date = 0;
		HwInfoData.year = MfgData->prod_year;
		if (HwInfoData.year == 0xFFFF) HwInfoData.year = 0;
		SendTele( TID_HW_INFO, &HwInfoData, sizeof(HwInfoData), rxb );
	}
	else
	{
		// Abfrage der Firmwareinfos
		rxb->tid = TID_FW_INFO;
		memset( &FwInfoData, 0, sizeof(FwInfoData) );
		FwInfoData.Tx_Size = TX_BUF_SIZE;
		FwInfoData.Rx_Size = RX_BUF_SIZE;
		FwInfoData.Version = VERSION_NR;
		strncpy(dat, app_info.compile_date, sizeof(dat));
		strncpy(tim, app_info.compile_time, sizeof(tim));

		// Update-Zeit zusammenbauen
		strncpy(zs, tim, 2);
		i=atoi(zs)*100;
		strncpy(zs, &tim[3], 2);
		i+=atoi(zs);
		FwInfoData.Upd_Time = i;

		// Update-Datum zusammenbauen
		strncpy(zs, &dat[4], 2);
		i=atoi(zs)*100;
		strncpy(zs, dat, 3);
		if ( (zs[0] == 'J') && (zs[1] == 'a') ) i+=1;
		if (zs[0] == 'F') i+=2;
		if ( (zs[0] == 'M') && (zs[2] == 'r') ) i+=3;
		if ( (zs[0] == 'A') && (zs[1] == 'p') ) i+=4;
		if ( (zs[0] == 'M') && (zs[2] == 'y') ) i+=5;
		if ( (zs[0] == 'J') && (zs[1] == 'u') && (zs[2] == 'n') ) i+=6;
		if ( (zs[0] == 'J') && (zs[2] == 'l') ) i+=7;
		if (( zs[0] == 'A') && (zs[1] == 'u') ) i+=8;
		if (zs[0] == 'S') i+=9;
		if (zs[0] == 'O') i+=10;
		if (zs[0] == 'N') i+=11;
		if (zs[0] == 'D') i+=12;
		FwInfoData.Upd_Date = i;

		// Update-Jahr zusammenbauen
		strncpy(zs, &dat[7], 4);
		i=atoi(zs);
		FwInfoData.Upd_Year = i;

		// Daten ausgeben
		SendTele( TID_FW_INFO, &FwInfoData, sizeof(FwInfoData), rxb );
	}
}


/******************************************************************************************************
                                           DoTeleGetPowerState
liest die relevanten Statusinformationen der externen Stromversorgungen und die Akkuinformationen. Wenn
der Akku kalibriert wurde, werden die Flash-Daten der letzten Kalibrierung für die Berechnung des aktu-
ellen Akkustatus verwendet, ansonsten wird die Standardtabelle verwendet.
******************************************************************************************************/
void DoTeleGetPowerState( RxBufStrct *rxb )
{
	u16 InpCurrVal;
	u16 ChgCurrVal;

	if (rxb->tid == TID_GET_BATTERY_STATE)
	{
		// Batteriedaten aus Flash und Ram ausgeben
		BatState.IntCalibCapacity = BatData->calib_capacity;
		if (BatState.IntCalibCapacity == 0xFFFF) { BatState.IntCalibCapacity = 0; BatState.IntCalibState = CALIB_NOT_DONE; }
		else { BatState.IntCalibState = CALIB_DONE; }
		BatState.DummyTime = 0;
		BatState.IntCalibUnixTime = BatData->calib_unix_time;
		if (BatState.IntCalibUnixTime == 0xFFFFFFFF) BatState.IntCalibUnixTime = 0;
		BatState.IntCurrent = AnalogVals.Akku_mA_LT;
		BatState.IntVoltage = AnalogVals.Akku_mV_LT;
		BatState.ExtChrgMode = 0; //CHRGMODE_UNDEFINED;		// ToDo!
		BatState.ExtChrgState = 0; //sizeof(BatState);		// ToDo!
		
		// !!!!! DEBUG !!!!! 
		/*
		BatState.ExtChrgState  = (u16)BatData->res_array[0];
		if (BatState.IntChrgState < 100)
			BatState.ExtChrgMode   = (u16)StdAkkuIntRes[BatState.IntChrgState];
		else
			BatState.ExtChrgMode = 0; 
		*/
		
		SendTele( TID_BATTERY_STATE, &BatState, sizeof(BatState), rxb );
	}
	else
	{
		// Externen Versorgungsstatus ausgeben
		ExtPwrState.In1Voltage = AnalogVals.In1_mV;
		ExtPwrState.In2Voltage = AnalogVals.In2_mV;
		ExtPwrState.UsbVoltage = AnalogVals.Usb_mV;
		ExtPwrState.PowerState = ChargerType;
		InpCurrVal = I2C_Data.Bq24192.InpSrcContr;
		ChgCurrVal = I2C_Data.Bq24192.ChgCurrContr;

		// Zuordnungen Eingangsstroeme
		switch (InpCurrVal)
		{
			case INP_CURR_100MA:  ExtPwrState.MaxInpCurrent = 100;  break;
			case INP_CURR_150MA:  ExtPwrState.MaxInpCurrent = 150;  break;
			case INP_CURR_500MA:  ExtPwrState.MaxInpCurrent = 500;  break;
			case INP_CURR_900MA:  ExtPwrState.MaxInpCurrent = 900;  break;
			case INP_CURR_1200MA: ExtPwrState.MaxInpCurrent = 1200; break;
			case INP_CURR_1500MA: ExtPwrState.MaxInpCurrent = 1500; break;
			case INP_CURR_2000MA: ExtPwrState.MaxInpCurrent = 2000; break;
			case INP_CURR_3000MA: ExtPwrState.MaxInpCurrent = 3000; break;
			default: 			  ExtPwrState.MaxInpCurrent = 0;    break;
		}

		// zuordnungen Ladestroeme
		switch (ChgCurrVal)
		{
			case CHAR_CURR_512MA:  ExtPwrState.MaxChrgCurrent = 512;  break;
			case CHAR_CURR_998MA:  ExtPwrState.MaxChrgCurrent = 998;  break;
			case CHAR_CURR_1344MA: ExtPwrState.MaxChrgCurrent = 1344; break;
			case CHAR_CURR_1408MA: ExtPwrState.MaxChrgCurrent = 1408; break;
			case CHAR_CURR_1792MA: ExtPwrState.MaxChrgCurrent = 1792; break;
			case CHAR_CURR_2048MA: ExtPwrState.MaxChrgCurrent = 2048; break;
			case CHAR_CURR_2304MA: ExtPwrState.MaxChrgCurrent = 2304; break;
			case CHAR_CURR_2752MA: ExtPwrState.MaxChrgCurrent = 2752; break;
			case CHAR_CURR_3008MA: ExtPwrState.MaxChrgCurrent = 3008; break;
			case CHAR_CURR_3520MA: ExtPwrState.MaxChrgCurrent = 3520; break;
			case CHAR_CURR_3840MA: ExtPwrState.MaxChrgCurrent = 3840; break;
			case CHAR_CURR_4032MA: ExtPwrState.MaxChrgCurrent = 4032; break;
			default:               ExtPwrState.MaxChrgCurrent = 0;    break;
		}

		// kleine Spannungen ausblenden
		if (ExtPwrState.In1Voltage < 500) ExtPwrState.In1Voltage = 0;
		if (ExtPwrState.In2Voltage < 500) ExtPwrState.In2Voltage = 0;
		if (ExtPwrState.UsbVoltage < 500) ExtPwrState.UsbVoltage = 0;

		SendTele( TID_EXTPOWER_STATE, &ExtPwrState, sizeof(ExtPwrState), rxb );
	}
}


/******************************************************************************************************
                                           DoTeleRgbLedState
stellt die LEDs per Telegramm ein bzw. liest die eingestellten Werte aus. Achtung: die Batterie-LEDs
sollen eigentlich nur vom STM32 bedient werden!
******************************************************************************************************/
void DoTeleRgbLedState( RxBufStrct *rxb )
{
	if (rxb->tid == TID_SET_RGBLED_STATE)
	{
		// RGB-LED-Struktur fuer Datenempfang
#pragma pack(2)
		typedef struct
		{
			u16 GenBright;
			u16 Mask;
			u32 FinalOffTime;
			u32	PerOnTime;
			u32 PerOffTime;
			u16 RedBright;
			u16 GreenBright;
			u16 BlueBright;
		} rx_strct;
#pragma pack()

		rx_strct *rx;
		s16 err = 0;

		rx = (rx_strct *)rxb->rdptr;
		if ( rxb->cnt == 11 * sizeof(u16) )     		// korrekte Laenge
		{
			if (rx->GenBright < 1002)
			{
				RgbLedState.GeneralBrightness = rx->GenBright;

				if (RgbLedState.GeneralBrightness == 1001)
				{
					if (DispBacklight.Mode == 1) SetDispLedBrightness( AUTOLEDSDISP, 0, 0 );
					else SetDispLedBrightness( AUTOLEDS, DispBacklight.Brightness, 0 );
				}
				else
				{
					if (DispBacklight.Mode == 1) SetDispLedBrightness( AUTODISP, 0, RgbLedState.GeneralBrightness );
					else SetDispLedBrightness( NOAUTO, DispBacklight.Brightness, RgbLedState.GeneralBrightness );
				}
			}

			SetLedState(rx->Mask, rx->RedBright, rx->GreenBright, rx->BlueBright,
					    rx->FinalOffTime, rx->PerOnTime, rx->PerOffTime );

			SendTeleAck( rxb );
		}
		else
		{ SendTeleNack( TERR_UNKNOWN_TELE, err, rxb ); }
	}

	// LED-Informationen zurueckgeben
	if (rxb->tid == TID_GET_RGBLED_STATE)
	{
		SendTele( TID_RGBLED_STATE, &RgbLedState.FinalOffTime, 168, rxb );
	}
}


/******************************************************************************************************
                                           DoTeleFlashLedState
stellt die rueckseitigen, weissen LEDs per Telegramm ein bzw. liest die eingestellten Werte aus.
******************************************************************************************************/
void DoTeleFlashLedState( RxBufStrct *rxb )
{
	// Struktur fuer Beleuchtungs-LED-Datenempfang
#pragma pack(2)
	typedef struct
	{
		u16 Brightness1;
		u16 Brightness2;
		u16 ModeMask;
		u16	PerTime;
		u16 OnTime;
	} rx_strct;
#pragma pack()

	if (rxb->tid == TID_SET_FLASHLED_STATE)
	{
		// Flash-LED einstellen
		rx_strct *rx;

		rx = (rx_strct *)rxb->rdptr;

		if ( rxb->cnt == 5 * sizeof(u16) )     		// korrekte Laenge
		{
			FlashLedState.BrightComp[0] = rx->Brightness1;
			FlashLedState.BrightComp[1] = rx->Brightness2;
			FlashLedState.ModeMask = rx->ModeMask;
			FlashLedState.OnTime = rx->OnTime;
			FlashLedState.PeriodicTime = rx->PerTime;

			WLed1OffTime = 0xFFFFFFFF;
			WLed2OffTime = 0xFFFFFFFF;
			SetFlashLedMode ();
			SendTeleAck( rxb );

		}
		else
		{ SendTeleNack( TERR_UNKNOWN_TELE, -1, rxb ); }
	}

	// Flash-LED-Einstellungen zurueckgeben
	if (rxb->tid == TID_GET_FLASHLED_STATE)
	{ SendTele( TID_FLASHLED_STATE, &FlashLedState, sizeof(FlashLedState), rxb ); }
}


/******************************************************************************************************
                                           DoTeleDispBrightness
stellt den Modus und die Helligkeit der Display-Hintergrundbeleuchtung ein
******************************************************************************************************/
void DoTeleDispBrightness( RxBufStrct *rxb )
{
	if (rxb->tid == TID_SET_DISP_BRIGHTNESS)
	{
		// Display-Helligkeit einstellen
		DispBacklightStruct *Db;

		Db = (DispBacklightStruct *)rxb->rdptr;
		if ( rxb->cnt == 2 * sizeof(u16) )     		// korrekte Laenge
		{
			DispBacklight.Mode = Db->Mode;
			DispBacklight.Brightness = Db->Brightness;

			if (RgbLedState.GeneralBrightness == 1001)
			{
				if (Db->Mode == 1) SetDispLedBrightness( AUTOLEDSDISP, 0, 0 );
				else SetDispLedBrightness( AUTOLEDS, Db->Brightness, 0 );
			}
			else
			{
				if (Db->Mode == 1) SetDispLedBrightness( AUTODISP, 0, RgbLedState.GeneralBrightness );
				else SetDispLedBrightness( AUTOLEDS, Db->Brightness, RgbLedState.GeneralBrightness );
			}
			SendTeleAck( rxb );
		}
		else
		{ SendTeleNack( TERR_UNKNOWN_TELE, 0, rxb ); }
	}
	else
	{
	// Display-Helligkeit zurueckgeben
#pragma pack(2)
		struct
		{
			u16 mode;
			u16 brightness;
			u16 ambient;
		} GBStruct;
#pragma pack()

		memset( &GBStruct, 0, sizeof(GBStruct) );

		DoSysAdMeasurements();
		GBStruct.ambient = AnalogVals.Ambient_AD;
		GBStruct.brightness = DispBacklight.Brightness;
		GBStruct.mode = DispBacklight.Mode;
		SendTele( TID_DISP_BRIGHTNESS, &GBStruct, sizeof(GBStruct), rxb );
	}
}

/******************************************************************************************************
                                           DoTeleBeep
laesst den Piezo mit einer vorgegebenen Dauer und Frequenz piepsen
******************************************************************************************************/
void DoTeleBeep( RxBufStrct *rxb )
{
	typedef struct
	{
		u16 length;
		u16 frequency;
	} rx_strct;

	rx_strct *rx;

	rx = (rx_strct *)rxb->rdptr;
	if ( rxb->cnt == 2 * sizeof(u16) )     		// korrekte Laenge
	{
		Beep( rx->frequency, rx->length );
		SendTeleAck( rxb );
	}
	else
	{ SendTeleNack( TERR_UNKNOWN_TELE, 0, rxb ); }
}


/******************************************************************************************************
                                           DoTeleRtc
stellt die RTC im STM32 bzw. liest die RTC aus und gibt die Zeit zurück
******************************************************************************************************/
void DoTeleRtc( RxBufStrct *rxb )
{
	if (rxb->tid == TID_GET_RTC)
	{
		// RTC-Zeit auslesen & zurueckgeben
		u32 RtcTime;

		RTC_GetTime( &RtcTime );
		SendTele( TID_RTC, &RtcTime, sizeof(u32), rxb );
	}

	if (rxb->tid == TID_SET_RTC)
	{
		// RTC-Zeit einstellen
		u32 *RtcTime;
		RtcTime = (u32 *)rxb->rdptr;

		if ( rxb->cnt == sizeof(u32) )     		// korrekte Laenge
		{
			RTC_SetTime (*RtcTime);
			SendTeleAck( rxb );
		}
		else
		{ SendTeleNack( TERR_UNKNOWN_TELE, 1, rxb ); }
	}

}


/******************************************************************************************************
                                                                                        DoTeleGetCrc
-------------------------------------------------------------------------------------------------------
   Berechnet die Crc von app_start_str bis app_stop_str (exclusive) und sendet ihn zurück

   Parameter:
      rxb:      Zeigt auf die Sendestruktur

******************************************************************************************************/
void DoTeleGetCrc( RxBufStrct *rxb )
{
   char *MinAdr, *MaxAdr;
   u32  Len;
   u16 CrcApp;

   // CRC-Daten: Schleife ueber gesamten RAM
   MinAdr = (char *)&app_info;
   MaxAdr = MinAdr + 128*1024;
   if ( (app_info.app_start_str < MinAdr) OR (app_info.app_start_str > MaxAdr)  OR
      (app_info.app_stop_str < MinAdr) OR (app_info.app_stop_str > MaxAdr) )
   { CrcApp = 0; }
   else
   {
      Len = (app_info.app_stop_str - app_info.app_start_str);
      CrcApp = CalcCrc16( 0, app_info.app_start_str, Len );
   }

   SendTele( TID_PROD_FW_CRC, &CrcApp, sizeof(CrcApp), rxb );
}


/******************************************************************************************************
                                           DoTeleEraseFlash
******************************************************************************************************/
void DoTeleEraseFlash( RxBufStrct *rxb )
{
#pragma pack(2)
	typedef  struct
	{
		u32 magic;
		u8  *start;
		u8  *end;
	} rx_strct;
#pragma pack(2)

	rx_strct *rx;
	s16 err = 1;

	rx = (rx_strct *)rxb->rdptr;
	if ( rxb->cnt == 3 * sizeof(u32) )
	{
		err = 2;
		if ( rx->magic == (DANGEROUS_TELE_MAGIC | ERFLASH_TELE_MAGIC) )
			err = EraseFlash( rx->start, rx->end );
	}

	if ( 0 == err ) SendTeleAck( rxb );
	else SendTeleNack( TERR_ERASEFLASH, err, rxb );
}

/******************************************************************************************************
                                          DoTeleWriteSegment
******************************************************************************************************/
void DoTeleWriteSegment( RxBufStrct *rxb )
{
#pragma pack(2)
	typedef  struct
	{
		u32 magic;
		u8  *dest;
		u32 len;          // Länge in Byte
		u8  data;
	} rx_strct;
#pragma pack()

	rx_strct *rx;
	s16 err = 1;

	rx = (rx_strct *)rxb->rdptr;
	if ( rxb->cnt >= 3 * sizeof(u32) )     		// magic + dest + len muss da sein
	{
		err = 2;
		if ( rx->magic == (DANGEROUS_TELE_MAGIC | WRFLASH_TELE_MAGIC) )
		{
			err = 3;
			if ( rxb->cnt == 3 * sizeof(u32) + rx->len )
				err = WriteFlash( &(rx->data), &(rx->dest), rx->len );
		}
	}

	if ( 0 == err ) SendTeleAck( rxb );
	else SendTeleNack( TERR_WRITEFLASH, err, rxb );
}

/******************************************************************************************************
                                           DoTeleHwProdInfo
liest und schreibt die (geheimen) Produktionsdaten aus dem / ins Flash
******************************************************************************************************/
void DoTeleHwProdInfo( RxBufStrct *rxb )
{
	if (rxb->tid == TID_PROD_GET_HW_MODULMASK)
	{
		// USB-C-Erkennung per default aktiv!
		rxb->tid = TID_PROD_HW_MODULMASK;
		ModMaskData.available_mask = MfgData->available_mask;
		if (ModMaskData.available_mask == 0xFFFF) ModMaskData.available_mask = 0x067F;
		ModMaskData.activated_mask = MfgData->activated_mask;
		if (ModMaskData.activated_mask == 0xFFFF) ModMaskData.activated_mask = 0x067F;
		SendTele( TID_PROD_HW_MODULMASK, &ModMaskData, sizeof(ModMaskData), rxb );
	}

	if (rxb->tid == TID_PROD_PROG_HWINFO)
	{
		// Hardwareinfo ins Flash schreiben
		s16 err = 0;

		ModMaskStrct *rx;
		rx = (ModMaskStrct *)rxb->rdptr;

		if ( rxb->cnt == 6 * sizeof(u16) )     		// auf korrekte Laenge checken
		{
			if (EraseFlash( (void *)MfgData, (void *)MfgData + sizeof(MfgData) ) ==0 )
			{
				err = WriteFlash( rx, (void *)MfgData, 6 * sizeof(u16) );//sizeof(MfgData) );
				if (err==0) SendTeleAck( rxb );
				else SendTeleNack( TERR_WRITEFLASH, err, rxb );
			}
			else { SendTeleNack( TERR_WRITEFLASH, err, rxb ); }
		}
		else
		{ SendTeleNack( TERR_UNKNOWN_TELE, 1, rxb ); }

	}

}


/******************************************************************************************************
                                           DoTeleStartBatCal
startet die (lange dauernde) Batteriekalibrierung und schreibt Ergebnisse ins Flash
******************************************************************************************************/
void DoTeleStartBatCal( RxBufStrct *rxb )
{
	u32 Time=0;
	u16 n = 0, Value = 0;
	s8 err=0;
	u8 *p_end_state, end_state;
	u8 i=0, Tm4, DoneCount = 0;
	p_end_state = (u8 *)rxb->rdptr;
	end_state = *p_end_state;

	DoneCount = 0;

	if ( rxb->cnt == sizeof(u8) )
	{
		// Wenn genau ein Byte Daten angekommen ist: Kalibrierung starten
		SendTeleAck( rxb );							// erste Bestaetigung sofort senden!
		SetDispLedBrightness( NOAUTO, 100, 100 );	// Helligkeitsregelung initialisieren
		BM_FullCharging ();
		err = BM_DisCharging ();
		BM_PartCharging ( end_state );

		// Warten, bis Akku auf den vorgewaehlten Ladezustand aufgeladen ist
		do
		{
			DelayUsRaw (1000000);

			if ( (Time%60) == 20)
			{
				DoSysAdMeasurements();
				GetBatLevel();
				BatState.IntVoltage = AnalogVals.Akku_mV_LT;
				BatState.IntCurrent = AnalogVals.Akku_mA_LT;
				BatState.IntCalibState = CALIB_PARTCHARGE;
				TIM_SetCompare1( TIM17, 100);

#ifdef ACCUDEBUG
				DoneCount += 2;
				DbgPrintf ("\r\n%dP-Laden. U = %d, I = %d, CS = %d, DC = %d", endstate, BatState.IntVoltage, BatState.IntCurrent, BatState.IntCalibState, DoneCount);
#else
				SendTeleCh( TID_BATTERY_STATE, &BatState, sizeof(BatState), COM_CHANNEL_RS );
#endif

				DelayMs (50);
				while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
				DelayMs (50);
				SETSOURCE_EXT;
				SETCHARGE;
			}

			CheckChargingMode();
			DoSysAdMeasurements();
			GetBatLevel();
			Value += BatState.IntChrgState;
			Tm4 = Time % 4;
			if ( Tm4 == 0 ) { TIM_SetCompare2( TIM1, 250 ); TIM_SetCompare3( TIM1, 150 ); TIM_SetCompare1( TIM4, 0 ); TIM_SetCompare2( TIM4, 0 ); TIM_SetCompare4( TIM4, 0 ); TIM_SetCompare1( TIM3, 0 ); }
			if ( ( Tm4 == 1 ) || ( Tm4 == 3 ) ) { TIM_SetCompare2( TIM1, 0 ); TIM_SetCompare3( TIM1, 0 ); TIM_SetCompare1( TIM4, 250 ); TIM_SetCompare2( TIM4, 150 ); TIM_SetCompare4( TIM4, 0 ); TIM_SetCompare1( TIM3, 0 ); }
			if ( Tm4 == 2) { TIM_SetCompare2( TIM1, 0 ); TIM_SetCompare3( TIM1, 0); TIM_SetCompare1( TIM4, 0 ); TIM_SetCompare2( TIM4, 0); TIM_SetCompare4( TIM4, 250); TIM_SetCompare1( TIM3, 150); }
			TIM_SetCompare1( TIM1, 0 ); TIM_SetCompare4( TIM1, 0 ); TIM_SetCompare3( TIM4, 0 );
			n++; Time++;

#ifndef ACCUDEBUG
			if (n >= 10)
			{
				n=0;
				if ( ( (Value/10) >= end_state) || (Time >= 72000) ) DoneCount++;
				Value = 0;
			}
#endif

		} while(DoneCount<6);
		Time = 0;

		//err = BatteryMeasurement( end_state );		// Messungen durchfuehren (kann seehr lange dauern!!!)
#ifndef ACCUDEBUG
		if (err == 0) SendTeleAck( rxb );			// wenn Messungen abgeschlossen sind: zweite Bestaetigung oder Fehlermeldung senden
		else if (err == 1) SendTeleNack ( TERR_CHARGING, 0, rxb );
		else if (err == -3) SendTeleNack ( TERR_ERASEFLASH, 0, rxb );
		else if (err == -4) SendTeleNack ( TERR_WRITEFLASH, 0, rxb );
		else SendTeleNack ( TERR_TIMEOUT, 0, rxb );
#endif

		while (1)
		{
			// nach Abschluss der Kalibrierung: in einer Endlosschleife bleiben
			DelayUsRaw (500000);
			I2C_Data.Bq24192.InpSrcContr = INP_CURR_1500MA;
			I2C_Data.Bq24192.PwrOnConf = 0x47;
			I2C_Data.Bq24192.ChgCurrContr = CHAR_CURR_512MA;
			i=0; err = 1;
			while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
			SETSOURCE_EXT;
			SETNOCHARGE;

			// LEDs zeigen abwechselnd RGB
			Tm4 = Time % 3;
			if ( Tm4 == 0)
			{
				TIM_SetCompare1(TIM1, 400); TIM_SetCompare2(TIM1, 0); TIM_SetCompare3(TIM1, 0);
				TIM_SetCompare4(TIM1, 400); TIM_SetCompare1(TIM4, 0); TIM_SetCompare2(TIM4, 0);
				TIM_SetCompare3(TIM4, 400); TIM_SetCompare4(TIM4, 0); TIM_SetCompare1(TIM3, 0);
			}
			if (Tm4 == 1)
			{
				TIM_SetCompare1(TIM1, 0); TIM_SetCompare2(TIM1, 400); TIM_SetCompare3(TIM1, 0);
				TIM_SetCompare4(TIM1, 0); TIM_SetCompare1(TIM4, 400); TIM_SetCompare2(TIM4, 0);
				TIM_SetCompare3(TIM4, 0); TIM_SetCompare4(TIM4, 400); TIM_SetCompare1(TIM3, 0);
			}
			if ( Tm4 == 2)
			{
				TIM_SetCompare1(TIM1, 0); TIM_SetCompare2(TIM1, 0); TIM_SetCompare3(TIM1, 400);
				TIM_SetCompare4(TIM1, 0); TIM_SetCompare1(TIM4, 0);	TIM_SetCompare2(TIM4, 400);
				TIM_SetCompare3(TIM4, 0); TIM_SetCompare4(TIM4, 0); TIM_SetCompare1(TIM3, 400);
			}

			if ( (Time%60) == 10)
			{
				DoSysAdMeasurements();
				GetBatLevel();
				if ( (AnalogVals.In1_mV > 3000) || (AnalogVals.In2_mV > 3000) || (AnalogVals.Usb_mV > 3000) )
				{ BatState.IntChrgMode = CHRGMODE_DONE; }
				else
				{ BatState.IntChrgMode = CHRGMODE_NOCHRG; }

				BatState.IntCurrent = AnalogVals.Akku_mA_LT;
				BatState.IntVoltage = AnalogVals.Akku_mV_LT;
				BatState.IntCalibState = CALIB_DONE;

#ifdef ACCUDEBUG
				DbgPrintf ("\r\nZyklusende. U = %d, I = %d, CS = %d", BatState.IntVoltage, BatState.IntCurrent, BatState.IntCalibState);
#else
				SendTeleCh( TID_BATTERY_STATE, &BatState, sizeof(BatState), COM_CHANNEL_RS );
#endif

				while ( (err!=0) && (i<5) ) { err = I2C_Write (BQ24192, BQ_INP_SRC, 3); i++; }
				TIM_SetCompare1( TIM17, 500);
			}
			Time++;
		}
	}
	else
	{ SendTeleNack( TERR_UNKNOWN_TELE, 1, rxb ); }
}


void DoTeleResend( RxBufStrct *rxb )
{
}


void DoTeleGetLastError( RxBufStrct *rxb )
{
}


/******************************************************************************************************
                                           DoTeleGetTemperature
Liefert alle vom STM32 messbaren Temperaturdaten zurück
******************************************************************************************************/
void DoTeleGetTemperature( RxBufStrct *rxb )
{
	s8 err;

	// Struktur fuer Temperaturwerte
	struct 									// Struktur fuer Temperaturwerte
	{
		s16 heatsink;
		s16 akku1;
		s16 akku2;
		s16 ledboard;
	} TempStruct;
	memset( &TempStruct, 0, sizeof(TempStruct) );

	// -100°C, wenn Temperaturen nicht gelesen werden koennen
	TempStruct.heatsink = -1000;
	TempStruct.akku1 = -1000;
	TempStruct.akku2 = -1000;
	TempStruct.ledboard = -1000;
	TempStruct.heatsink = AnalogVals.TempKuehlDeg;
	if (AnalogVals.TempAkku1Raw > 4000) { TempStruct.akku1 = -1000; }
	else { TempStruct.akku1 = AnalogVals.TempAkku1Deg; }
	if (AnalogVals.TempAkku2Raw > 4000) { TempStruct.akku2 = -1000; }
	else { TempStruct.akku2 = AnalogVals.TempAkku2Deg; }

	// Temperatur an der LED-Platine lesen
	err = I2C_Read (LM75, LM_RD_TEMP, 2);
	if (err==0) TempStruct.ledboard = I2C_Data.Lm75.Temperature&0x00FF; // Daten kommen vertauscht an!
	else TempStruct.ledboard = -1000;
	if ( (TempStruct.ledboard & 0x0100) && (err == 0) ) TempStruct.ledboard ^= -TempStruct.ledboard;

	SendTele( TID_TEMPERATURE, &TempStruct, sizeof(TempStruct), rxb );
}



/******************************************************************************************************
                                           DoTeleGetDebugData
liest Debug-Daten (IO-Werte, Analogwerte, D/A-Werte, Speicherinhalte) aus dem STM32 aus und gibt sie
per Telegramm zurück
******************************************************************************************************/
void DoTeleGetDebugData( RxBufStrct *rxb )
{
	switch (rxb->tid)
	{
		// digitale IOs lesen
		case TID_DEBUG_GET_DIGITAL_IOS:
		{
#pragma pack(2)
			struct
			{
				u16 input_mask[5];
				u16 output_mask[5];
				u16 state_mask[5];
			} DigIoStruct;
#pragma pack()

			memset( &DigIoStruct, 0, sizeof(DigIoStruct) );

			int i;
			u16 bitmask = 1;
			u8 arraycount = 0;

			for (i=0; i<80; i++)
			{
				// nur lesen, wenn die Pins als Eingaenge geschaltet sind
				if (  (PinMode[i] == GPIO_Mode_IPD) || (PinMode[i] == GPIO_Mode_IPU)
				   || (PinMode[i] == GPIO_Mode_IN_FLOATING) )
				{
					DigIoStruct.input_mask[arraycount] |= bitmask;
					switch (arraycount)
					{
						case 0: if (GPIO_ReadInputDataBit(GPIOA, i%16) == 1 ) DigIoStruct.state_mask[0] |= bitmask; break;
						case 1: if (GPIO_ReadInputDataBit(GPIOB, i%16) == 1 ) DigIoStruct.state_mask[1] |= bitmask; break;
						case 2: if (GPIO_ReadInputDataBit(GPIOC, i%16) == 1 ) DigIoStruct.state_mask[2] |= bitmask; break;
						case 3: if (GPIO_ReadInputDataBit(GPIOD, i%16) == 1 ) DigIoStruct.state_mask[3] |= bitmask; break;
						case 4: if (GPIO_ReadInputDataBit(GPIOE, i%16) == 1 ) DigIoStruct.state_mask[4] |= bitmask; break;
					}
				}

				if (  (PinMode[i] == GPIO_Mode_Out_PP) || (PinMode[i] == GPIO_Mode_Out_OD) )
				{
					DigIoStruct.output_mask[arraycount] |= bitmask;
					switch (arraycount)
					{
						case 0: if (GPIO_ReadOutputDataBit(GPIOA, i%16) == 1 ) DigIoStruct.state_mask[0] |= bitmask; break;
						case 1: if (GPIO_ReadOutputDataBit(GPIOB, i%16) == 1 ) DigIoStruct.state_mask[1] |= bitmask; break;
						case 2: if (GPIO_ReadOutputDataBit(GPIOC, i%16) == 1 ) DigIoStruct.state_mask[2] |= bitmask; break;
						case 3: if (GPIO_ReadOutputDataBit(GPIOD, i%16) == 1 ) DigIoStruct.state_mask[3] |= bitmask; break;
						case 4: if (GPIO_ReadOutputDataBit(GPIOE, i%16) == 1 ) DigIoStruct.state_mask[4] |= bitmask; break;
					}
				}

				if ( (i%16) == 15)
				{ bitmask = 1; arraycount += 1; }
				else
				{ bitmask <<= 1; }
			}
			SendTele( TID_DEBUG_DIGITAL_IOS, &DigIoStruct, sizeof(DigIoStruct), rxb );
		}
		break;

		// analoge Werte lesen
		case TID_DEBUG_ANALOG_VALS:
		{
#pragma pack(2)
			struct
			{
				u16 input_mask;
				u16 output_mask;
				u16 value[16];
			} AnalogIoStruct;
#pragma pack()

			memset( &AnalogIoStruct, 0, sizeof(AnalogIoStruct) );

			u8 n, i;
			static u8 d=0;
			u16 bitmask = 1;

			for (i=0; i<16; i++)
			{
				switch (i)
				{
					case 0: case 1: case 2: case 3:
					case 4:	case 5:	case 6:	case 7:
						n = i;
					break;

					case 8: case 9:
						n = i + 8;
					break;

					case 10: case 11: case 12:
					case 13: case 14: case 15:
						n = i + 22;
					break;
				}

				if (PinMode[n] == GPIO_Mode_AIN)
				{
					AnalogIoStruct.input_mask |= bitmask;
					ADC_RegularChannelConfig(ADC1, i, 1, ADC_SampleTime_239Cycles5);
					ADC_SoftwareStartConvCmd(ADC1, ENABLE);
					while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
					AnalogIoStruct.value[i] = ADC_GetConversionValue(ADC1);
				}

				if (PinMode[n] == GPIO_Mode_AF_PP)
				{
					AnalogIoStruct.output_mask |= bitmask;
					if (d==0) AnalogIoStruct.value[i] = DAC_GetDataOutputValue(DAC_Channel_1);
					if (d==1) AnalogIoStruct.value[i] = DAC_GetDataOutputValue(DAC_Channel_2);
					if (d==0) d=1; else d=0;
				}
				bitmask <<= 1;
			}
			SendTele( TID_DEBUG_ANALOG_VALS, &AnalogIoStruct, sizeof(AnalogIoStruct), rxb );
		}
		break;

		// Speicherbereich lesen
		case TID_DEBUG_GET_MEMORY_AREA:
		{
			u8 i;
			u32 *ptr;

#pragma pack(1)
			typedef struct
			{
				u32 StartAddress;
				u8  Size;
			} rx_strct;
#pragma pack()

			rx_strct *rx;
			u32 Data[128];
			memset( &Data, 0, sizeof(Data) );
			rx = (rx_strct *)rxb->rdptr;

			if ( rxb->cnt == 5 * sizeof(u8) )
			{
				ptr = (u32 *)rx->StartAddress;
				if (rx->Size > 128) rx->Size = 128;

				if ( ( (rx->StartAddress >= 0x08000000) && (rx->StartAddress <= 0x0801FFFC) )
				  || ( (rx->StartAddress >= 0x20000000) && (rx->StartAddress <= 0x20001FFC) )
				  || ( (rx->StartAddress >= 0x40000000) && (rx->StartAddress <= 0x400233FC) ) )
				{
					for (i=0; i < rx->Size; i++) Data[i] = *ptr++;
					SendTele( TID_DEBUG_MEMORY_AREA, &Data, i*sizeof(u32), rxb );
				}
				else
				{ SendTeleNack( TERR_ADRESSERROR, 1, rxb ); }
			}
			else
			{ SendTeleNack( TERR_UNKNOWN_TELE, 1, rxb ); }
		}
		break;
	}
}
