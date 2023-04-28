#include "system.h"
#include "syslib.h"
#include "globals.h"
#include "I2C.h"
#include "sage.h"

I2C_DataStrct I2C_Data;

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : Initialisiert den I2C-Controller für die Kommunikation mit
* 				   dem USB-Erkennungs-IC und mit dem Akkulade-IC.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_Configuration ( void )
{
	I2C_InitTypeDef I2C_Conf;

	I2C_Conf.I2C_Mode = I2C_Mode_I2C;
	I2C_Conf.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Conf.I2C_OwnAddress1 = 0;
	I2C_Conf.I2C_Ack = I2C_Ack_Enable;
	I2C_Conf.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Conf.I2C_ClockSpeed = 200000;
	I2C_Init(I2C1, &I2C_Conf);
	I2C_Cmd(I2C1, ENABLE);
}

/******************************************************************************************************
                                          I2C_Init_IC
-------------------------------------------------------------------------------------------------------
initialisiert den USB3751-Baustein (USB-Erkennung) bz. den BQ24192 auf sinnvolle Werte. Achtung: vor
der Initialisierung des USB3751 wird geprueft, ob die USB-Spannung (= Betriebsspannung des USB3751)
vorhanden ist. Deshalb geht die Initialisierung des USB3751 nur mit initialisiertem und laufendem
AD-Wandler. Beim BQ24192 ist die USB-Spannung nicht erforderlich & wird nicht überprüft.
******************************************************************************************************/
s8 I2C_Init_IC (u8 IC)
{
//	s8 err;

	if (IC == LM75)
	{
/*		I2C_Data.Lm75.Config = 0;
		err = I2C_Write (LM75, LM_CONF, 1);
		if (err != 0) return err;
		I2C_Data.Lm75.TAlarmHyst = 40 << 7; // 40°C := 0,5*80°C, linksbuendig, positiv
		err = I2C_Write (LM75, LM_THYST, 2);
		if (err != 0) return err;
		I2C_Data.Lm75.TAlarm = 100 << 7;	// 100°C := 0,5*200°C, linksbuendig, positiv
		return I2C_Write (LM75, LM_TOS, 2); */
		return 0;
	}

	else if (IC == BQ24192)
	{
		I2C_Data.Bq24192.InpSrcContr = 0x0A; 		I2C_Data.Bq24192.PwrOnConf = 0x17;
		I2C_Data.Bq24192.ChgCurrContr = 0x00;		I2C_Data.Bq24192.PreChgTermCurrContr = 0x11;
		I2C_Data.Bq24192.ChgVoltContr = 0xB0;		I2C_Data.Bq24192.ChgTermTimContr = 0x8C;
		I2C_Data.Bq24192.IrCompThermRegContr = 0x03;I2C_Data.Bq24192.MiscOpContr = 0x4B;

		return I2C_Write (BQ24192, BQ_INP_SRC, 8);
	}

	else if (IC == USB3751)
	{
		I2C_Data.Usb3751.Status = 0x00; 			I2C_Data.Usb3751.Config = 0x80;
		I2C_Data.Usb3751.MeasContr1 = 0x00;			I2C_Data.Usb3751.MeasContr2 = 0x00;

		if (AnalogVals.Usb_mV >= I2C_MINVOLTAGE) { return I2C_Write (USB3751, USB_CONFIG, 4); }
		else { return 1; }
	}
	else { return 2; }
}

/******************************************************************************************************
                                          I2C_SingleRead
-------------------------------------------------------------------------------------------------------
liest ein Byte vom angegebenen IC von der angegebenen Adresse. Achtung: vor dem Lesen von Werten
aus dem USB3751-Baustein wird geprueft, ob die USB-Spannung vorhanden ist. Deshalb geht die Initia-
lisierung nur mit initialisiertem und laufendem AD-Wandler.
******************************************************************************************************/
s8 I2C_Read (u8 IC, u8 RegAdr, u8 Size)
{
	u16 Timeout;
	u8  Nr=RegAdr, *ptr, EndAdr;

	EndAdr = RegAdr + Size;
	if (EndAdr > sizeof(I2C_DataStrct) ) return 2;
	if (Size > sizeof(I2C_Data)) return 3;
	if ( (AnalogVals.Usb_mV < I2C_MINVOLTAGE) && (IC == USB3751) ) { return 1; }
	if ( ( ( HwModuleMask.OnOff & 0x0200) == 0) && (IC == LM75) ) { return 4; }

	if (IC == USB3751) { ptr = (u8 *)&I2C_Data.Usb3751; }
	else if (IC == BQ24192) { ptr = (u8 *)&I2C_Data.Bq24192; }
	else if (IC == LM75)    { ptr = (u8 *)&I2C_Data.Lm75; }
	else return 5;

	I2C_AcknowledgeConfig(I2C1, ENABLE);

	Timeout = I2C_TIMEOUT;
	while ( (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && (--Timeout > 0));

	if (Timeout) { I2C_GenerateSTART(I2C1, ENABLE); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -1; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (--Timeout > 0));

	if (Timeout) { I2C_Send7bitAddress(I2C1, IC<<1, I2C_Direction_Transmitter); }
	else
	{ I2C_SoftwareResetCmd (I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -2; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (--Timeout > 0));

	if (Timeout) { I2C_SendData(I2C1, RegAdr); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -3; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (--Timeout > 0));

	if (Timeout) { I2C_GenerateSTOP(I2C1, ENABLE); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -4; }
	Timeout = I2C_TIMEOUT;
	while ( (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY)) && (--Timeout) );

	if (Timeout) { I2C_GenerateSTART(I2C1, ENABLE); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -5; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB) && (--Timeout > 0)));

	if (Timeout) { I2C_Send7bitAddress(I2C1, IC<<1, I2C_Direction_Receiver); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -6; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (--Timeout > 0));
	if (!Timeout) { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -7; }

	while ( (Nr < EndAdr) && (--Timeout > 0) )
	{
		if (Nr == (EndAdr-1)) { I2C_AcknowledgeConfig(I2C1, DISABLE); }
		Timeout = I2C_TIMEOUT;
		while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (--Timeout > 0));
		if (Timeout) { ptr[Nr] = I2C_ReceiveData(I2C1); }
		else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -8-Nr; }
		Nr++;
	}

	if (Timeout) I2C_GenerateSTOP(I2C1, ENABLE);
	Timeout = I2C_TIMEOUT;
	while ( (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY)) && (--Timeout) );
	if (Timeout) { return 0; }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -8-Nr; }
	return 0;
}

/******************************************************************************************************
                                          I2C_SingleWrite
-------------------------------------------------------------------------------------------------------
schreibt ein uebergebenes Byte an das angegebene IC an die uebergebene Adresse
******************************************************************************************************/
s8 I2C_Write (u8 IC, u8 RegAdr, u8 Size)
{
	u16 Timeout;
	u8  Nr=RegAdr, *ptr, EndAdr;

	EndAdr = RegAdr + Size;
	if (EndAdr > sizeof(I2C_DataStrct) ) return -1;
	if ( (AnalogVals.Usb_mV < I2C_MINVOLTAGE) && (IC == USB3751) ) { return 1; }
	if ( ( ( HwModuleMask.OnOff & 0x0200) == 0) && (IC == LM75) ) { return 4; }

	if (IC == USB3751) { ptr = (u8 *)&I2C_Data.Usb3751; }
	else if (IC == BQ24192) { ptr = (u8 *)&I2C_Data.Bq24192; }
	else if (IC == LM75)    { ptr = (u8 *)&I2C_Data.Lm75; }
	else return 5;

	Timeout = I2C_TIMEOUT;
	while ( (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && (--Timeout) );

	if (Timeout) { I2C_GenerateSTART(I2C1, ENABLE); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -2; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (--Timeout) );

	if (Timeout) { I2C_Send7bitAddress(I2C1, IC<<1, I2C_Direction_Transmitter); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -3; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (--Timeout) );

	if (Timeout) { I2C_SendData(I2C1, RegAdr); }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -4; }
	Timeout = I2C_TIMEOUT;
	while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (--Timeout) );

	while ( (Nr < EndAdr) && (--Timeout > 0) )
	{
		Timeout = I2C_TIMEOUT;
		I2C_SendData(I2C1, ptr[Nr]);
		while ( (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (--Timeout) );
		if (!Timeout) { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -5-Nr; }
		Nr++;
	}

	if (Timeout) I2C_GenerateSTOP(I2C1, ENABLE);
	Timeout = I2C_TIMEOUT;
	while ( (I2C_GetFlagStatus (I2C1, I2C_FLAG_BUSY)) && (--Timeout) );
	if (Timeout) { return 0; }
	else { I2C_SoftwareResetCmd(I2C1, ENABLE); I2C_SoftwareResetCmd(I2C1, DISABLE); return -5-Nr; }
	return 0;
}

