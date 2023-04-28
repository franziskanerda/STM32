#ifndef __GLOBALS_H
#define __GLOBALS_H 1

/**********   #INCLUDES            **********/
/**********   #DEFINES             **********/
#define MFGDATA_MAGIC   0x5A3CA500

#define SYS_STAT_PWRSTATUS           0  // Gesetzt nach Reset+PowerOn, reset durch TID_RESETPOWERSTATUS
#define SYS_STAT_BEAMFIND			 2	// Gesetzt wenn Laser im Beamfindermodus laeuft
#define SYS_STAT_LASERERR            5  // Gesetzt wenn der Laser in ErrorBlink steht
#define SYS_STAT_LINERR              9  // Gesetzt wenn Linearisierungsdaten fehlerhaft
#define SYS_STAT_BOOTCORRUPT        17  // Wird gesetzt wenn die crc des bootloaders nicht stimmt

#define SYS_STAT_LASERBATT_MASK     0x0000F000
#define SYS_STAT_LASERBATT_SHIFT    12
#define SYS_STAT_LASERBATT_0        12  // Batteriestatus Laser 4 Bits
#define SYS_STAT_LASERBATT_1        13  // Batteriestatus Laser 4 Bits
#define SYS_STAT_LASERBATT_2        14  // Batteriestatus Laser 4 Bits
#define SYS_STAT_LASERBATT_3        15  // Batteriestatus Laser 4 Bits

#define BATTSTAT_10            0
#define BATTSTAT_20            1
#define BATTSTAT_30            2
#define BATTSTAT_40            3
#define BATTSTAT_50            4
#define BATTSTAT_60            5
#define BATTSTAT_70            6
#define BATTSTAT_80            7
#define BATTSTAT_90            8
#define BATTSTAT_100           9
#define BATTSTAT_CHARGEFAST   10
#define BATTSTAT_CHARGEFULL   11
#define BATTSTAT_CHARGEEND    12
#define BATTSTAT_CHARGEERR    13
#define BATTSTAT_EXTPWR       14
#define BATTSTAT_UNKNOWN      15
#define BATTSTAT_NEAR_EMPTY1  16		// intern verwendet; Ausgabe: nur letzte vier Bits!
#define BATTSTAT_NEAR_EMPTY2  17
#define BATTSTAT_EMPTY1		  32
#define BATTSTAT_EMPTY2		  33
#define BATTSTAT_SWITCH_OFF	  64

/**********   ENUMS                **********/
// Typdefinition fuer alte Kabelversionen


/**********   STRUCTS              **********/
typedef struct MfgDataStrct_s
{
   u16  available_mask;		// Maske, die die verfuegbaren Module enthaelt
   u16  activated_mask;		// Maske, die die freigeschaltete Module enthaelt
   u16  prod_date;         	// Produktionsdatum. Format: ddmm dh. 108 dezimal bedeutet 1.August
   u16  prod_year;        	// Produktionsjahr. Format: yyyy dh. 1999 beudeutet 1999
   u32  serno;              // Seriennummer
   u8   reserved[512 /*- 2 * sizeof(u32)*/ - 5 * sizeof(u16)];  // auf 512 Bytes füllen
   u16  crc;
} MfgDataStrct;

typedef struct ModMaskStrct_s
{
   u16  available_mask;		// Maske, die die verfuegbaren Module enthaelt
   u16  activated_mask;		// Maske, die die freigeschaltete Module enthaelt
   u16  prod_date;         	// Produktionsdatum. Format: ddmm dh. 108 dezimal bedeutet 1.August
   u16  prod_year;        	// Produktionsjahr. Format: yyyy dh. 1999 beudeutet 1999
   u32  serno;              // Seriennummer
} ModMaskStrct;

typedef struct BatDataStrct_s
{
   u32  mfgmagic1;          // magic word 1
   u32  calib_unix_time;   	// Zeit der zuletzt durchgefuehrten Kalibrierung
   u16  time_dummy;			// erhaelt die Kompatibilitaet zum alten Protokoll
   u16  calib_capacity;		// Akkukapazitaet in mAh; Stand: letzte Kalibrierung
   u16  voltage_array[100];	// Nullstromspannungs-Feld fuer die eingebauten Akkus
   u16  res_array[100];		// Innenwiderstands-Feld fuer die eingebauten Akkus
   u32  mfgmagic2;          // magic word 2
   u8   reserved[512 - 3 * sizeof(u32) - 203 * sizeof(u16)];  // auf 512 Bytes füllen
   u16  crc;
} BatDataStrct;

typedef struct HwInfoDataStrct_s
{
	u16  hws_runf;          // Hardwarestand RunF (aus A/D-Wert)
	u16  hws_akku;         	// Hardwarestand Akku (aus A/D-Wert)
	u16  date;             	// Produktionsdatum. Format: ddmm dh. 108 dezimal bedeutet 1.August
	u16  year;             	// Produktionsjahr. Format: yyyy dh. 2014 beudeutet 2014
	u32  serno;            	// Seriennummer
	u8   reserved[32 - 1 * sizeof(u32) - 4 * sizeof(u16) - 0 * sizeof(u8)];  // auf 32 füllen
} HwInfoDataStrct;


/**********   CONSTS               **********/

/**********   Public Variables     **********/
extern const BatDataStrct *const BatData;
extern const MfgDataStrct *const MfgData;
extern HwInfoDataStrct HwInfoData;
extern ModMaskStrct  ModMaskData;
extern volatile u32 sys_tic;
extern u32 sys_status;
extern u8  PowerState;
extern u8  DetectedPower;
extern u8  DetectedChange;
extern u16 BatVoltAdc;
extern u16 IchgAdc;
extern u32 LastCommTime;
extern s16 GlobalMinTemp;
extern s16 GlobalMaxTemp;
extern u16 GlobalEvent;

/**********   Public Functions     **********/
void SetSysStatus_LaserBattStatus( u8 BattStatus );

#endif   // __GLOBALS_H
