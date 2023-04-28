#ifndef I2C_H_
#define I2C_H_

#define USB3751			0x68	// I2C-Adresse USB3751
#define LM75			0x69	// I2C-Adresse LM75 (LED-Platine)
#define BQ24192			0x6B	// I2C-Adresse BQ24192

// USB3751-Register
#define USB_STATUS		0x00	// Ueberspannung / 100mA-Limit / erkannter USB-Lader
#define USB_CONFIG		0x01	// Ueberspannungseinstellung / Reset
#define USB_CHG_CONT	0x02	// direkte Einstellmoeglichkeit fuer Erkennungsschaltung
#define USB_CHG_STAT	0x03	// direkte Erkennung von Analogwerten

// BQ24192-Register
#define BQ_INP_SRC		0x00	// Limit Eingangsspannung / Eingangsstrombegrenzung
#define BQ_PWRON_CONF	0x01	// Verhalten beim Einschalten
#define BQ_CHAR_CURR	0x02	// Ladestrom
#define BQ_PRECH_CURR	0x03	// Ladestrombegrenzung Precharge / Termination
#define BQ_CHAR_VOLT	0x04	// Ladeschlussspannung (3,5 V ... 4,4 V)
#define BQ_CHAR_TERM	0x05	// Timereinstellungen für Ladevorgang
#define BQ_IRTEMP_COMP	0x06	// Akku-Impedanzanpassung / Temperaturbereich
#define BQ_MISC_CONT	0x07	// Fehlerbehandlung / Akkuabschaltung /...
#define BQ_RD_STATUS	0x08	// Eingangsspannung / Ladezustand / Fehlerstatus
#define BQ_RD_ERROR		0x09	// Fehlerwert
#define BQ_RD_DEVICE	0x0A	// Baustein-Typ

// LM75-Register
#define LM_RD_TEMP		0x01	// Temperatur-Register

#define I2C_TIMEOUT 	750		// Warteschleifenzaehler bis Timeout
#define I2C_MINVOLTAGE	4000	//4500 // Mindestspannung in mV fuer USB-Chip-Erkennung

#pragma pack(1)
typedef struct BQ24192_s
{
	u8 InpSrcContr;
	u8 PwrOnConf;
	u8 ChgCurrContr;
	u8 PreChgTermCurrContr;
	u8 ChgVoltContr;
	u8 ChgTermTimContr;
	u8 IrCompThermRegContr;
	u8 MiscOpContr;
	u8 SysStatus;
	u8 SysFault;
	u8 VendorPartRevStatus;
}Bq24192Strct;
#pragma pack()

typedef struct USB3751_s
{
	u8 Status;
	u8 Config;
	u8 MeasContr1;
	u8 MeasContr2;
}Usb3751Strct;

typedef struct LM75_s
{
	u8 	Pointer;
	u16 Temperature;
	u8  Config;
	u16 TAlarmHyst;
	u16 TAlarm;
}Lm75Strct;

typedef struct I2C_Data_s
{
	Bq24192Strct Bq24192;
	Usb3751Strct Usb3751;
	Lm75Strct   Lm75;
}I2C_DataStrct;

extern I2C_DataStrct I2C_Data;

void I2C_Configuration ( void );
s8 I2C_Init_IC (u8 IC);
s8 I2C_Read (u8 IC, u8 RegAdr, u8 Size);
s8 I2C_Write (u8 IC, u8 RegAdr, u8 Size);

#endif /* I2C_H_ */
