#ifndef __TELES_H
#define __TELES_H 1

// Die Werte der Specialcharacters in den Telegrammen:
#define RS_START_CHAR	0x10  // bei Änderung der define-Werte müssen die Encoding und
#define RS_XON_CHAR		0x11  // Decoding Quellcodes evtl. angepasst werden!!
#define RS_ESC_CHAR		0x12
#define RS_XOFF_CHAR	0x13
#define RS_STOP_CHAR	0x14

// Der Wert um den zu kodierende Bytes verschoben werden
#define RS_ESC_OFFSET	0x40

#define DANGEROUS_TELE_MAGIC	0xA53C5300
#define RESET_TELE_MAGIC		0x00000001
#define ERFLASH_TELE_MAGIC		0x00000002
#define WRFLASH_TELE_MAGIC		0x00000003
#define SWITCH_OFF_TELE_MAGIC	0x00000005
#define FW_UPDATE_MAGIC			0x00000006

// Das Hibyte der Telegrammnummern kennzeichnen deren Gruppenzugehörigkeit
// Das LowByte dient zur Durchnummerierung innerhalb der Gruppe

// Allgemeine Telegramme
#define TID_ACK						0x0001		// Documented
#define TID_NACK					0x0002		// Documented
#define TID_RESEND_LAST_TELE		0x0004		// Documented
#define TID_GET_LAST_ERROR			0x0040		// Documented
#define TID_LAST_ERROR				0x0041		// Documented

// Event-Telegramme
#define TID_GET_EVENT				0x0050		// Documented
#define TID_EVENT					0x0051		// Documented

// Grundeinstellungen
#define TID_PROD_ERASEFLASH			0x0200		// not documented
#define TID_PROD_WRITEFLASH			0x0201		// not documented
#define TID_PROD_RESET				0x0202		// Documented
#define TID_PROD_GET_FW_CRC			0x0209		// Documented
#define TID_PROD_FW_CRC				0x020A		// Documented
#define TID_PROD_PROG_FW			0x0240		// Documented
#define TID_PROD_START_BATCALIB		0x0241		// Documented
#define TID_PROD_PROG_HWINFO		0x0242		// Documented
#define TID_PROD_GET_HW_MODULMASK	0x0243		// Documented
#define TID_PROD_HW_MODULMASK		0x0244		// Documented

// Modul-Schalter
#define TID_SET_HW_MODULES_ONOFF	0x0340		// Documented
#define TID_GET_HW_MODULES_ONOFF	0x0341		// Documented
#define TID_MODULES_ONOFF			0x0342		// Documented

// RunF-spezifische Infos
#define TID_GET_HW_INFO				0x0500		// Documented
#define TID_HW_INFO					0x0501		// Documented
#define TID_GET_FW_INFO				0x0502		// Documented
#define TID_FW_INFO					0x0503		// Documented
#define TID_GET_BATTERY_STATE		0x0504		// Documented
#define TID_BATTERY_STATE			0x0505		// Documented
#define TID_GET_EXTPOWER_STATE		0x0506		// Documented
#define TID_EXTPOWER_STATE			0x0507		// Documented
#define TID_SET_RGBLED_STATE		0x0508		// Documented
#define TID_GET_RGBLED_STATE		0x0509		// Documented
#define TID_RGBLED_STATE			0x050A		// Documented
#define TID_SET_FLASHLED_STATE		0x050B		// Documented
#define TID_GET_FLASHLED_STATE		0x050C		// Documented
#define TID_FLASHLED_STATE			0x050D		// Documented
#define TID_SET_DISP_BRIGHTNESS		0x050E		// Documented
#define TID_GET_DISP_BRIGHTNESS		0x0520		// Documented
#define TID_DISP_BRIGHTNESS			0x0521		// Documented
#define TID_SET_RTC					0x0522		// Documented
#define TID_GET_RTC					0x0523		// Documented
#define TID_RTC						0x0524		// Documented
#define TID_BEEP					0x0525		// Documented
#define TID_GET_TEMPERATURE			0x0526		// Documented
#define TID_TEMPERATURE				0x0527		// Documented

// Debug-Protokolle
#define TID_DEBUG_GET_DIGITAL_IOS 	0xF040		// Documented
#define TID_DEBUG_DIGITAL_IOS 		0xF041		// Documented
#define TID_DEBUG_GET_ANALOG_VALS 	0xF042		// Documented
#define TID_DEBUG_ANALOG_VALS 		0xF043		// Documented
#define TID_DEBUG_GET_MEMORY_AREA	0xF044		// Documented
#define TID_DEBUG_MEMORY_AREA		0xF045		// Documented

// NGS-Protokolle (koennen zum NGS oder NGL gesendet werden wenn Geraet aus ist).
#define TID_SWITCHOFF				0x0309		// works internal for RunF and can be sent to NGS/NGL
#define TID_SETCHARGING      		0x030D		// documented in NGS/NGL documentation

// Major error numbers for TID_NACK
#define TERR_MISC            	0
#define TERR_UNKNOWN_TELE     	1
#define TERR_BAD_CRC          	2
#define TERR_RESET            	4
#define TERR_ERASEFLASH       	5
#define TERR_WRITEFLASH       	6
#define TERR_BUSY             	7
#define TERR_ADRESSERROR		8
#define TERR_TIMEOUT			9
#define TERR_CHARGING			10
#define TERR_MODULESWITCH		11

#endif // ifndef __TELES_H
