#ifndef __INFO_H
#define __INFO_H 1

/**********   #INCLUDES            **********/

/**********   #DEFINES             **********/
#define VERSION_NR 104     		// Versionsnummer * 100   Beispiel: 100 = 1.00; ACHTUNG: keine fuehrende 0!
#define DEVICE_CLASS_ID 0x4300	// RunF (Rotalign touch)-ID 4300(h)
#define SERIALNUMBER 0x0001		// Seriennummer (wird in HW-Info ausgegeben)
#define PHW 140					// Platinenhardwarestand 1.40 (erste Version war 1.00)

/****************************************************************************************************
*  Die PHW geht sowohl aus diesem '#define PHW' als auch aus der Analogspannung hervor. Bei der 	*
*  Analogspannung werden 32 Stufen unterschieden. Die Stufen sind folgenden PHWs zugeordnet:		*
*																									*
*  Version		PHW			R50			R52			U(Version)										*
*  --------------------------------------------------------------------------------------------		*
*  1			1.00		10M			0R			0,000 V ( 2,5 V / 32 St * 0,5 (bzw. 0) )		*
*  2			1.10		1M			47K			0,117 V ( 2,5 V / 32 St * 1,5)					*
*  3			1.20		1M			86K6		0,195 V ( 2,5 V / 32 St * 2,5)					*
*  4			1.21		1M			121K		0,273 V	( 2,5 V / 32 St * 3,5)					*
*  5			1.30/31		1M			165K		0,352 V	( 2,5 V / 32 St * 4,5)					*
*  6			1.32		1M			205K		0,430 V ( 2,5 V / 32 St * 5,5)					*
*  7			1.33		1M			255K		0,508 V	( 2,5 V / 32 St * 6,5)					*
*  8			1.40		1M			309K		0,586 V ( 2,5 V / 32 St * 7,5)					*
*  ...																								*
*  32			(letzte)	0R			10M			2,500 V ( 2,5 V / 32 St * 31,5 (bzw. 32) )		*
*  --------------------------------------------------------------------------------------------		*
****************************************************************************************************/

#ifndef CRC_PLACE_HOLDER
   #define CRC_PLACE_HOLDER 0x3101  //Platzhalter, um die CRC im Intel Hexfile sicher zu finden.
#endif

/**********   ENUMS                **********/

/**********   STRUCTS              **********/
typedef struct 
{
   void (*entry_point)( void );
   u32  version;
   const char *app_start_str;
   const char *app_stop_str;
   char	compile_date[12];
   char	compile_time[12];
   u32 	rxbuf_size;
   u32	txbuf_size;
   u16 	crc;
   u8 reserved[64 - 6 * sizeof(u32) - 1 * sizeof(u16) - 12 - 12];   // auff�llen auf 64 Byte
} appinfo_strct;

extern const appinfo_strct app_info;

#endif // ifndef __INFO_H
