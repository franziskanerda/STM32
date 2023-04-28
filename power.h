#ifndef POWER_H_
#define POWER_H_

// wenn ACCUDEBUG definiert ist, kommen beim Akkutest allerlei Debug-Infos raus!!!
// Die Debug-Ausgaben koennen dann per 'cat /dev/ttymxc2 &' per RS232 ausgegeben werden
// #define ACCUDEBUG

// Definitionen fuer digitale Ausgaenge des Controllers
#define SW_3V3_ON		GPIO_SetBits   	( DOUT_3V3_ON )
#define SW_3V3_OFF		GPIO_ResetBits 	( DOUT_3V3_ON ) 
#define SW_5V_ON		GPIO_SetBits   	( DOUT_5V_ON )
#define SW_5V_OFF		GPIO_ResetBits 	( DOUT_5V_ON )

#define SENSOR_CHARGING	{ GPIO_SetBits	( DOUT_SENS_LOW ); 	GPIO_SetBits( DOUT_SENS_HIGH );   }
#define SENSOR_ON		{ GPIO_SetBits	( DOUT_SENS_LOW ); 	GPIO_ResetBits( DOUT_SENS_HIGH ); }
#define SENSOR_OFF		{ GPIO_ResetBits( DOUT_SENS_LOW );	GPIO_ResetBits( DOUT_SENS_HIGH ); }

#define DISPLAY_ON		GPIO_SetBits   	( DOUT_DISP_ON )
#define DISPLAY_OFF		GPIO_ResetBits 	( DOUT_DISP_ON )
#define CAMERA_ON		GPIO_SetBits   	( DOUT_CAM_ON )
#define CAMERA_OFF		GPIO_ResetBits 	( DOUT_CAM_ON )
#define BLUETOOTH_ON	GPIO_SetBits   	( DOUT_BT_ON )
#define BLUETOOTH_OFF	GPIO_ResetBits 	( DOUT_BT_ON )
#define WLAN_ON			GPIO_SetBits   	( DOUT_WLAN_ON )
#define WLAN_OFF		GPIO_ResetBits 	( DOUT_WLAN_ON )
#define TOUCH_ON		GPIO_SetBits   	( DOUT_TOUCH_ON )
#define TOUCH_OFF		GPIO_ResetBits 	( DOUT_TOUCH_ON ) 
#define NFC_ON			GPIO_SetBits   	( DOUT_NFC_ON )
#define NFC_OFF			GPIO_ResetBits 	( DOUT_NFC_ON )
#define MICRO_SD_ON		GPIO_SetBits   	( DOUT_SD_ON )
#define MICRO_SD_OFF	GPIO_ResetBits 	( DOUT_SD_ON ) 
#define MEMS_ON			GPIO_SetBits   	( DOUT_MEMS_ON )
#define MEMS_OFF		GPIO_ResetBits 	( DOUT_MEMS_ON )
#define WLED_ON			GPIO_SetBits   	( DOUT_WLEDS_ON )
#define WLED_OFF		GPIO_ResetBits  ( DOUT_WLEDS_ON )
#define CAM_RST			GPIO_ResetBits  ( DIO_NRSTCAM )
#define CAM_NRST		GPIO_SetBits  	( DIO_NRSTCAM )

#define IMX6_STM1_ON	GPIO_SetBits   	( DIO_IMX6_STM1 )
#define IMX6_STM1_OFF	GPIO_ResetBits	( DIO_IMX6_STM1 )
#define IMX6_STM2_ON	GPIO_SetBits   	( DIO_IMX6_STM2 )
#define IMX6_STM2_OFF	GPIO_ResetBits	( DIO_IMX6_STM2 )
#define IMX6_STM3_ON	GPIO_SetBits   	( DIO_IMX6_STM3 )
#define IMX6_STM3_OFF	GPIO_ResetBits	( DIO_IMX6_STM3 )
#define IMX6_STM4_ON	GPIO_SetBits   	( DIO_IMX6_STM4 )
#define IMX6_STM4_OFF	GPIO_ResetBits	( DIO_IMX6_STM4 )
#define IMX6_ERWPIN_ON	GPIO_SetBits   	( DIO_ERWPIN )
#define IMX6_ERWPIN_OFF	GPIO_ResetBits  ( DIO_ERWPIN )

#define STM_FLASH1_ON	GPIO_SetBits   	( PWM_FLASHSW1 )
#define STM_FLASH1_OFF	GPIO_ResetBits	( PWM_FLASHSW1 )
#define STM_FLASH2_ON	GPIO_SetBits   	( PWM_FLASHSW2 )	
#define STM_FLASH2_OFF	GPIO_ResetBits	( PWM_FLASHSW2 )

#define SETNOCHARGE		GPIO_SetBits	( DOUT_nBQ_CE )
#define SETCHARGE		GPIO_ResetBits	( DOUT_nBQ_CE )
#define SETSOURCE_USB	GPIO_SetBits	( DOUT_BQ_PSEL )
#define SETSOURCE_EXT	GPIO_ResetBits	( DOUT_BQ_PSEL )

// Definitionen fuer digitale Eingaenge des Controllers
#define DET_PWRON			( GPIO_ReadInputDataBit ( DIN_PIC_PWRON ) )
#define DET_ONOFF_KEY		( GPIO_ReadInputDataBit ( DIN_PIC_KEY_OUT ) )
#define DET_BQ24192_NPGOOD  ( GPIO_ReadInputDataBit( DIN_nBQ_PGOOD ) )
#define DET_IMX6_PGOOD 		( GPIO_ReadInputDataBit ( DIN_IMX6_PWRGOOD ) )
#define DET_BQ24192_STAT	( GPIO_ReadInputDataBit ( DIN_nBQ_PGOOD ) )
#define DET_RS232_CONNECT	( GPIO_ReadInputDataBit ( DIN_MAX_RS232_CONN ) )
#define DET_CHARGING		( GPIO_ReadInputDataBit ( DIN_BQ_STAT ) )
#define DET_USB3751_INT		( !GPIO_ReadInputDataBit ( DIN_nUSB3751_INT ) )
#define DET_BQ_INT			( !GPIO_ReadInputDataBit ( DIN_BQ_INT ) )

#define DET_OC_IMX6			( !GPIO_ReadInputDataBit( DIN_nSW_OC_IMX6 ) )
#define DET_OC_SENS			( !GPIO_ReadInputDataBit( DIN_nSW_OC_SENS ) )
#define DET_OC_DISP_TOUCH	( !GPIO_ReadInputDataBit( DIN_nSW_OC_DISP_TCH ) )
#define DET_OC_FUNK			( !GPIO_ReadInputDataBit( DIN_nSW_OC_FUNK ))
#define DET_OC_SD_MEMS		( !GPIO_ReadInputDataBit( DIN_nSW_OC_SD_MEMS ) )
#define DET_OC_NFC			( !GPIO_ReadInputDataBit( DIN_nSW_OC_NFC ))
#define DET_OC_WLED			( !GPIO_ReadInputDataBit( DIN_nSW_OC_WLED ))
#define DET_OVERCURRENT		(DET_OC_IMX6<<6) | (DET_OC_SENS<<5) | (DET_OC_DISP_TOUCH<<4) | (DET_OC_FUNK<<3) | (DET_OC_SD_MEMS<<2) | (DET_OC_NFC<<1) | DET_OC_WLED;

#define DET_TEMPALARM		( GPIO_ReadInputDataBit ( DIN_TEMPALARM ) )
#define DET_TEMPPWROFF		( !GPIO_ReadInputDataBit ( DIN_nTEMP_PWROFF ) )

#define DET_IMX6_STM1 		(GPIO_ReadInputDataBit( DIO_IMX6_STM1 ))
#define DET_IMX6_STM2 		(GPIO_ReadInputDataBit( DIO_IMX6_STM2 ))
#define DET_IMX6_STM3 		(GPIO_ReadInputDataBit( DIO_IMX6_STM3 ))
#define DET_IMX6_STM4 		(GPIO_ReadInputDataBit( DIO_IMX6_STM4 ))
#define DET_IMX6_STM		(DET_IMX6_STM4<<3) | (DET_IMX6_STM3<<2) | (DET_IMX6_STM2<<1) | DET_IMX6_STM1

// Allgemeine Definitionen
#define OFF							0
#define ON							1

// Betriebszustaende des RunF
#define	POWER_UNKNOWN				0
#define POWEROFF_NOTCHARGING_INIT	1
#define POWEROFF_NOTCHARGING_LOOP	2
#define POWEROFF_CHARGING_INIT		3
#define POWEROFF_CHARGING_INIT2		4
#define POWEROFF_CHARGING_INIT3		5
#define POWEROFF_CHARGING_LOOP		6
#define POWERON_NOTCHARGING_INIT	7
#define POWERON_NOTCHARGING_LOOP	8
#define POWERON_CHARGING_INIT		9
#define POWERON_CHARGING_INIT2		10
#define POWERON_CHARGING_INIT3		11
#define POWERON_CHARGING_LOOP		12

// Ueberstrom-Zustaende
#define OC_NONE						0x00
#define OC_WLED						0x01
#define OC_NFC						0x02
#define OC_SD_MEMS					0x04
#define OC_FUNK						0x08
#define OC_DISP_TOUCH				0x10
#define OC_SENS						0x20
#define OC_IMX6						0x40

// durch den STM32 direkt erkannte Ladegeraete
#define CHRG_NODETECT		0x01	// noch nichts erkannt
#define CHRG_NOCHARGER		0x00	// kein (bekanntes) Ladegeraet entdeckt (nicht laden)
#define CHRG_DETERROR		0x02	// fehlerhaftes oder unbekanntes Netzteil
#define CHRG_EXTNT_5V		0x04	// externes Netzteil mit ca. 5V (max. 1 A)
#define CHRG_EXTNT_9V		0x08	// externes Netzteil mit ca. 9V (max. 3 A)
#define CHRG_EXTNT_12V		0x10	// externes Netzteil mit ca. 12V (max. 2 A)

// durch den USB3751 per STM32 erkannte USB-Ladegeraete
#define CHRG_DCP			0x20	// Dedicated Charger Port (max. 1,8 A) / Android
#define CHRG_CDP			0x40	// Charging Downstream Port (max. 1,5 A) / USB-NT
#define CHRG_SDP			0x60	// Standard Downstream Port (max. 0,5 A) / PC
#define CHRG_SE1_LOW		0x80	// SE1 Low Current Charger (max. 0,5 A) / USB-NT
#define CHRG_SE1_HIGH		0xA0	// SE1 High Current Charger (max. 1 A) / iPhone
#define CHRG_IPAD			0xC0	// iPad (undokumentierte Direkterkennung!) (max. 2 A)

// erkannte Ladezustaende
#define CHRGMODE_UNDEFINED	0x01
#define CHRGMODE_NOCHRG		0x00
#define CHRGMODE_LOWVOLT	0x02
#define CHRGMODE_PRECHRG	0x04
#define CHRGMODE_FAST		0x08
#define CHRGMODE_DONE		0x10
#define CHRGMODE_THERMREG	0x20

// Kalibrierstati
#define CALIB_NOT_DONE		0
#define CALIB_FULLCHARGE	1
#define CALIB_DISCHARGE		2
#define CALIB_PARTCHARGE	3
#define CALIB_DONE			4
#define CALIB_NOK			5

// Akkuzustandsschwellen in % des Ladezustands
#define ABS_EMPTY 			 2
#define EMPTY				10
#define LOWMED				40
#define MEDHIGH				70

// Verwendbare Eingangsstroeme (BQ24192)
#define INP_CURR_100MA		0x08
#define INP_CURR_150MA		0x09
#define INP_CURR_500MA		0x0A
#define INP_CURR_900MA		0x0B
#define INP_CURR_1200MA		0x0C
#define INP_CURR_1500MA		0x0D
#define INP_CURR_2000MA		0x0E
#define INP_CURR_3000MA		0x0F

// Einstellbare Ladestroeme (BQ24192)
#define CHAR_CURR_512MA		0x01
#define CHAR_CURR_525MA		0x05
#define CHAR_CURR_538MA		0x09
#define CHAR_CURR_550MA		0x0D
#define CHAR_CURR_563MA		0x11
#define CHAR_CURR_576MA		0x15
#define CHAR_CURR_589MA		0x19
#define CHAR_CURR_602MA		0x1D
#define CHAR_CURR_614MA		0x21
#define CHAR_CURR_627MA		0x25
#define CHAR_CURR_640MA		0x29
#define CHAR_CURR_653MA		0x2D
#define CHAR_CURR_666MA		0x31
#define CHAR_CURR_678MA		0x35
#define CHAR_CURR_691MA		0x39
#define CHAR_CURR_704MA		0x3D
#define CHAR_CURR_717MA		0x41
#define CHAR_CURR_730MA		0x45
#define CHAR_CURR_742MA		0x49
#define CHAR_CURR_755MA		0x4D
#define CHAR_CURR_768MA		0x51
#define CHAR_CURR_781MA		0x55
#define CHAR_CURR_794MA		0x59
#define CHAR_CURR_806MA		0x5D
#define CHAR_CURR_819MA		0x61
#define CHAR_CURR_832MA		0x65
#define CHAR_CURR_845MA		0x69
#define CHAR_CURR_858MA		0x6D
#define CHAR_CURR_870MA		0x71
#define CHAR_CURR_883MA		0x75
#define CHAR_CURR_896MA		0x79
#define CHAR_CURR_909MA		0x7D
#define CHAR_CURR_922MA		0x81
#define CHAR_CURR_934MA		0x85
#define CHAR_CURR_947MA		0x89
#define CHAR_CURR_960MA		0x8D
#define CHAR_CURR_973MA		0x91
#define CHAR_CURR_986MA		0x95
#define CHAR_CURR_998MA		0x99
#define CHAR_CURR_1011MA	0x9D
#define CHAR_CURR_1024MA	0xA1
#define CHAR_CURR_1037MA	0xA5
#define CHAR_CURR_1050MA	0xA9
#define CHAR_CURR_1062MA	0xAD
#define CHAR_CURR_1075MA	0xB1
#define CHAR_CURR_1088MA	0xB5
#define CHAR_CURR_1101MA	0xB9
#define CHAR_CURR_1114MA	0xBD
#define CHAR_CURR_1126MA	0xC1
#define CHAR_CURR_1139MA	0xC5
#define CHAR_CURR_1152MA	0xC9
#define CHAR_CURR_1165MA	0xCD
#define CHAR_CURR_1178MA	0xD1
#define CHAR_CURR_1190MA	0xD5
#define CHAR_CURR_1203MA	0xD9
#define CHAR_CURR_1216MA	0xDD
#define CHAR_CURR_1229MA	0xE1
#define CHAR_CURR_1242MA	0xE5
#define CHAR_CURR_1254MA	0xE9
#define CHAR_CURR_1267MA	0xED
#define CHAR_CURR_1280MA	0xF1
#define CHAR_CURR_1293MA	0xF5
#define CHAR_CURR_1306MA	0xF9
#define CHAR_CURR_1318MA	0xFD
#define CHAR_CURR_1344MA	0x34
#define CHAR_CURR_1408MA	0x38
#define CHAR_CURR_1472MA	0x3C
#define CHAR_CURR_1536MA	0x40
#define CHAR_CURR_1600MA	0x44
#define CHAR_CURR_1664MA	0x48
#define CHAR_CURR_1728MA	0x4C
#define CHAR_CURR_1792MA	0x50
#define CHAR_CURR_1856MA	0x54
#define CHAR_CURR_1920MA	0x58
#define CHAR_CURR_1984MA	0x5C
#define CHAR_CURR_2048MA	0x60
#define CHAR_CURR_2112MA	0x64
#define CHAR_CURR_2176MA	0x68
#define CHAR_CURR_2240MA	0x6C
#define CHAR_CURR_2304MA	0x70
#define CHAR_CURR_2368MA	0x74
#define CHAR_CURR_2432MA	0x78
#define CHAR_CURR_2496MA	0x7C
#define CHAR_CURR_2560MA	0x80
#define CHAR_CURR_2624MA	0x84
#define CHAR_CURR_2688MA	0x88
#define CHAR_CURR_2752MA	0x8C
#define CHAR_CURR_2816MA	0x90
#define CHAR_CURR_2880MA	0x94
#define CHAR_CURR_2944MA	0x98
#define CHAR_CURR_3008MA	0x9C
#define CHAR_CURR_3072MA	0xA0
#define CHAR_CURR_3136MA	0xA4
#define CHAR_CURR_3200MA	0xA8
#define CHAR_CURR_3264MA	0xAC
#define CHAR_CURR_3328MA	0xB0
#define CHAR_CURR_3392MA	0xB4
#define CHAR_CURR_3456MA	0xB8
#define CHAR_CURR_3520MA	0xBC
#define CHAR_CURR_3584MA	0xC0
#define CHAR_CURR_3648MA	0xC4
#define CHAR_CURR_3712MA	0xC8
#define CHAR_CURR_3776MA	0xCC
#define CHAR_CURR_3840MA	0xD0
#define CHAR_CURR_3904MA	0xD4
#define CHAR_CURR_3968MA	0xD8
#define CHAR_CURR_4032MA	0xDC
#define CHAR_CURR_4096MA	0xE0
#define CHAR_CURR_4160MA	0xE4
#define CHAR_CURR_4224MA	0xE8
#define CHAR_CURR_4288MA	0xEC
#define CHAR_CURR_4352MA	0xF0
#define CHAR_CURR_4416MA	0xF4
#define CHAR_CURR_4480MA	0xF8
#define CHAR_CURR_4544MA	0xFC

#define PRE_CURR_128MA		0x00
#define PRE_CURR_256MA		0x10
#define PRE_CURR_384MA		0x20
#define PRE_CURR_512MA		0x30
#define PRE_CURR_640MA		0x40
#define PRE_CURR_768MA		0x50
#define PRE_CURR_896MA		0x60
#define PRE_CURR_1024MA		0x70
#define PRE_CURR_1152MA		0x80
#define PRE_CURR_1280MA		0x90
#define PRE_CURR_1408MA		0xA0
#define PRE_CURR_1536MA		0xB0
#define PRE_CURR_1664MA		0xC0
#define PRE_CURR_1792MA		0xD0
#define PRE_CURR_1920MA		0xE0
#define PRE_CURR_2048MA		0xF0

#define TERM_CURR_128MA		0x00
#define TERM_CURR_256MA		0x01
#define TERM_CURR_384MA		0x02
#define TERM_CURR_512MA		0x03
#define TERM_CURR_640MA		0x04
#define TERM_CURR_768MA		0x05
#define TERM_CURR_896MA		0x06
#define TERM_CURR_1024MA	0x07
#define TERM_CURR_1152MA	0x08
#define TERM_CURR_1280MA	0x09
#define TERM_CURR_1408MA	0x0A
#define TERM_CURR_1536MA	0x0B
#define TERM_CURR_1664MA	0x0C
#define TERM_CURR_1792MA	0x0D
#define TERM_CURR_1920MA	0x0E
#define TERM_CURR_2048MA	0x0F

#define UNKNOWNVOLTAGE		0
#define UNDERVOLTAGE		1
#define VOLTAGE5			2
#define VOLTAGE9			3
#define VOLTAGE12			4
#define OVERVOLTAGE			5

#pragma pack(2)
typedef struct BatState_s
{
	u16 IntChrgMode;
	u16 IntChrgState;
	u16 IntVoltage;
	s16 IntCurrent;
	u16 IntCalibState;
	u16 IntCalibCapacity;
	u32 IntCalibUnixTime;
	u16 DummyTime;
	u16 ExtChrgMode;
	u16 ExtChrgState;
} BatStateStruct;
#pragma pack()

typedef struct ExtPwrState_s
{
	u16 PowerState;
	u16 MaxChrgCurrent;
	u16 In1Voltage;
	u16 In2Voltage;
	u16 UsbVoltage;
	u16 MaxInpCurrent;
} ExtPwrStateStruct;

extern BatStateStruct BatState;
extern ExtPwrStateStruct ExtPwrState;

extern const u16 StdAkkuThreas[];
extern const u8 StdAkkuIntRes[];

extern u8 PowerChangeDetect;
extern u8 ChargerType;
extern u8 OldChargingState;
extern u8 OcEvent;

void ModuleSwitch (void);
void PowerInit (void);
void PowerOff (void);
void PowerOffCharging (void);
void PowerOn (void);
void PowerOnCharging (void);
u8 GetPowerMode (void);
u8 CheckExtPower(void);
u8 CheckUsbSource(void);
void SetExtPowerMode(void);
void CheckChargingMode(void);
void GetBatLevel(void);
void KeyTest(void);
void OvercurrentTest(void);
s8   BatteryMeasurement(u8 endstate);
void BM_FullCharging (void);
s8 BM_DisCharging (void);
void BM_PartCharging (u8 endstate);

#endif /* POWER_H_ */
