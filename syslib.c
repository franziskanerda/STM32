/** Description        *******************************************************************************/
// Systemweite Hilfsfunktionen

/** #INCLUDES          *******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <limits.h>
//#include <time.h>
#include "system.h"
#include "globals.h"
#include "syslib.h"
#include "sage.h"
#include "task.h"
#include "serial.h"
#include "gpio_map.h"

/** #DEFINES           *******************************************************************************/
#define TANTBLSIZE 450
#define MAXCLKTIME 1000000

/** ENUMS              *******************************************************************************/

/** STRUCTS            *******************************************************************************/

/** CONSTS             *******************************************************************************/
// tantable[n] = tan((0.1� * n) + 0.05�) * 2^16,  n = 0..449, korrekt gerundet
const u16 tantable[TANTBLSIZE] = {
   57,    172,   286,   400,   515,   629,   744,   858,   972,   1087,  1201,  1316,  1430,  1544,
   1659,  1773,  1888,  2002,  2117,  2231,  2346,  2460,  2575,  2689,  2804,  2919,  3033,  3148,
   3263,  3377,  3492,  3607,  3721,  3836,  3951,  4066,  4181,  4295,  4410,  4525,  4640,  4755,
   4870,  4985,  5100,  5215,  5330,  5446,  5561,  5676,  5791,  5907,  6022,  6137,  6253,  6368,
   6484,  6599,  6715,  6830,  6946,  7062,  7177,  7293,  7409,  7525,  7641,  7757,  7873,  7989,
   8105,  8221,  8337,  8453,  8570,  8686,  8803,  8919,  9036,  9152,  9269,  9386,  9502,  9619,
   9736,  9853,  9970,  10087, 10204, 10321, 10439, 10556, 10673, 10791, 10908, 11026, 11143, 11261,
   11379, 11497, 11615, 11733, 11851, 11969, 12087, 12206, 12324, 12442, 12561, 12680, 12798, 12917,
   13036, 13155, 13274, 13393, 13512, 13632, 13751, 13870, 13990, 14110, 14229, 14349, 14469, 14589,
   14709, 14829, 14950, 15070, 15190, 15311, 15432, 15552, 15673, 15794, 15915, 16037, 16158, 16279,
   16401, 16522, 16644, 16766, 16888, 17010, 17132, 17254, 17377, 17499, 17622, 17744, 17867, 17990,
   18113, 18236, 18360, 18483, 18607, 18730, 18854, 18978, 19102, 19226, 19350, 19475, 19599, 19724,
   19849, 19974, 20099, 20224, 20349, 20475, 20601, 20726, 20852, 20978, 21104, 21231, 21357, 21484,
   21611, 21737, 21864, 21992, 22119, 22246, 22374, 22502, 22630, 22758, 22886, 23015, 23143, 23272,
   23401, 23530, 23659, 23788, 23918, 24048, 24178, 24308, 24438, 24568, 24699, 24829, 24960, 25091,
   25223, 25354, 25486, 25617, 25749, 25881, 26014, 26146, 26279, 26412, 26545, 26678, 26811, 26945,
   27079, 27213, 27347, 27482, 27616, 27751, 27886, 28021, 28156, 28292, 28428, 28564, 28700, 28837,
   28973, 29110, 29247, 29384, 29522, 29660, 29797, 29936, 30074, 30213, 30351, 30490, 30630, 30769,
   30909, 31049, 31189, 31329, 31470, 31611, 31752, 31893, 32035, 32177, 32319, 32461, 32604, 32746,
   32890, 33033, 33176, 33320, 33464, 33609, 33753, 33898, 34043, 34189, 34334, 34480, 34626, 34773,
   34920, 35066, 35214, 35361, 35509, 35657, 35806, 35954, 36103, 36252, 36402, 36552, 36702, 36852,
   37003, 37154, 37305, 37457, 37609, 37761, 37914, 38066, 38219, 38373, 38527, 38681, 38835, 38990,
   39145, 39300, 39456, 39612, 39768, 39925, 40082, 40239, 40397, 40555, 40713, 40872, 41031, 41190,
   41350, 41510, 41671, 41831, 41993, 42154, 42316, 42478, 42641, 42804, 42967, 43131, 43295, 43460,
   43624, 43790, 43955, 44121, 44288, 44455, 44622, 44789, 44957, 45126, 45295, 45464, 45634, 45804,
   45974, 46145, 46316, 46488, 46660, 46833, 47006, 47179, 47353, 47527, 47702, 47877, 48053, 48229,
   48406, 48583, 48760, 48938, 49116, 49295, 49475, 49654, 49835, 50015, 50197, 50378, 50561, 50743,
   50927, 51110, 51294, 51479, 51664, 51850, 52036, 52223, 52410, 52598, 52787, 52975, 53165, 53355,
   53545, 53736, 53928, 54120, 54312, 54506, 54699, 54894, 55089, 55284, 55480, 55677, 55874, 56072,
   56270, 56469, 56669, 56869, 57070, 57271, 57474, 57676, 57879, 58083, 58288, 58493, 58699, 58905,
   59113, 59320, 59529, 59738, 59948, 60158, 60369, 60581, 60793, 61006, 61220, 61435, 61650, 61866,
   62083, 62300, 62518, 62737, 62957, 63177, 63398, 63620, 63842, 64066, 64290, 64515, 64740, 64967,
   65194, 65422
};

/** Public Variables   *******************************************************************************/
/** Public Functions   *******************************************************************************/
RgbLedStateStrct RgbLedState;
FlashLedStateStrct FlashLedState;
AnalogValStrct AnalogVals;
HwModuleMaskStruct HwModuleMask;
DispBacklightStruct DispBacklight;
u32 BeepOffTime = 0;
u32 WLed1OffTime = 0;
u32 WLed2OffTime = 0;
GPIOMode_TypeDef PinMode[100];

u16 AmbVolt[16], AkkVolt[16], KuehlTemp[16], AkkVoltLt[16];
s16 AkkCurr[16], AkkCurrLt[16];
u16 UsbVolt[4],  In1Volt[4], In2Volt[4],  InpCurr[4];

/** Private Variables  *******************************************************************************/
static char DbgMsgStr[128];

/** Private Functions  *******************************************************************************/

/** Code               *******************************************************************************/

/******************************************************************************************************
                                                 wtoh
-------------------------------------------------------------------------------------------------------
Konvertiert einen 16Bit Wert in einen HexStr mit evtl. f�hrenden Nullen. z.B. 123 => "007B"
Der Ergebnissstring besteht aus 4 Zeichen und einer abschliessenden Null

Parameter:
   Value:    Zu konvertierender 16Bit Wert
   String:   Zeiger auf String in dem das Ergebniss abgelegt wird. Mindestens 5 Bytes lang.

Return: Zeiger auf Ergebnissstring
******************************************************************************************************/
u8 *wtoh( u16 Value, u8 *String )
{
	u8  *strp = String;
	u8  nibble;
	s32 n;

	for ( n = 12 ; n >= 0 ; n -= 4 )
	{
		nibble = (u8)( (Value >> n) & 0x0F );
		if ( nibble <= 9 )
			*(strp++) = (u8)('0' + nibble);
		else
			*(strp++) = (u8)('7' + nibble);
	}
	*strp = 0;   // Setzen einer abschliessenden '\0' !
	return String;
}

/******************************************************************************************************
                                                 dwtoh
-------------------------------------------------------------------------------------------------------
Konvertiert einen 32Bit Wert in einen HexStr mit f�hrenden Nullen. z.B. 2952662750 -> "AFFE0EDE"
Der Ergebnissstring besteht aus 8 Zeichen und einer abschliessenden Null

Parameter:
   Value:    Zu konvertierender 32Bit Wert
   String:   Zeiger auf String in dem das Ergebniss abgelegt wird. Mindestens 9 Bytes lang.

Return: Zeiger auf Ergebnissstring
******************************************************************************************************/
u8 *dwtoh( u32 Value, u8 *String )
{
	u8  *strp = String;
	u8  nibble;
	s32 n;

	for ( n = 28 ; n >= 0 ; n -= 4 )
	{
		nibble = (u8)( (Value >> n) & 0x0F );
		if ( nibble <= 9 )
			*(strp++) = (u8)('0' + nibble);
		else
			*(strp++) = (u8)('7' + nibble);
	}
	*strp = 0;   // Setzen einer abschliessenden '\0' !
	return String;
}

/******************************************************************************************************
                                                 htodw
-------------------------------------------------------------------------------------------------------
Konvertiert einen HexString in einen numerischen Wert. Der HexString kann zwischen 8 und 1 Zeichen lang
sein.

Parameter:
  String:   Zeiger auf String
  Len:      Anzahl der Zeichen die ausgewertet werden sollen. Max 8 Zeichen.

Return: Der numerische Wert des HexStrings
******************************************************************************************************/
u32 htodw( const u8 *String, u8 Len )
{
	u8  mul[8];
	u32 res;
	s32 n;
	u32 m;

	if ( Len > 8 ) Len = 8;

	for ( n = 0 ; n < Len ; n++ )
	{
		if ( (*String >= '0') AND (*String <= '9') )
		{ mul[n] = (u8)(*String - '0'); }
		else if ( (*String >= 'A') AND (*String <= 'F') )
		{ mul[n] = (u8)(*String - 'A' + 10); }
		else if ( (*String >= 'a') AND (*String <= 'f') )
		{ mul[n] = (u8)(*String - 'a' + 10); }
		String++;
	}
	m = 0;
	res = 0;
	for ( --n ; n >= 0 ; n-- )
	{
		res += ( ( (u32)(mul[n]) ) << m );
		m += 4;
	}
	return res;
}

/******************************************************************************************************
                                                DelayMs
-------------------------------------------------------------------------------------------------------
H�lt den Aufrufer f�r die angegebene Zeit in Millisekunden an. Die Statemachine l�uft weiter, der Task
der den Delay aufgerufen hat wird aber angehalten.

Parameter:
   Delay:    Wartezeit in Millisekunden
******************************************************************************************************/
void DelayMs( u16 Delay )
{
	u64 tic;
	u16 tasknr;

	tic = GetSystemUs() + Delay * 1000;

	tasknr = SuspendTask();
	do
	{
		TaskList();
	} while ( GetSystemUs() < tic );

	ResumeTask( tasknr );
}

/******************************************************************************************************
                                              DelayUsRaw
-------------------------------------------------------------------------------------------------------
H�lt den Aufrufer f�r die angegebene Zeit in Microsekunden an. Die Statemachine steht.

Parameter:
   Delay:    Wartezeit in �s
******************************************************************************************************/
void DelayUsRaw( u32 Delay )
{
	u64 tic;

	tic = GetSystemUs() + Delay;
	while ( GetSystemUs() < tic ) ;
}

/******************************************************************************************************
                                              GetSystemUs
-------------------------------------------------------------------------------------------------------
Liefert die Zeit in �s seit das System gestartet wurde.

Return: Zeit in �s seit Systemstart
******************************************************************************************************/
u64 GetSystemUs( void )
{
	u32 hw_tic1, hw_tic2;
	u32 sw_tic;
	u64 r;

	// Der HW-Z�hler wird zweimal gelesen, der vom IRQ hochgez�hlte ms-Z�hler dazwischen.
	// Wenn der zuletzt gelesene HW-Z�hler h�her ist als der zuerst gelesene (HW-Z�hler l�uft r�ckw�rts)
	// ist zwischen den Lesevorg�ngen ein Unterlauf aufgetreten. In dem Fall ist nicht garantiert
	// das der Sekundenz�hler synchron zum HW-Z�hler ist darum wird der Vorgang widerholt.
	do
	{
		hw_tic1 = SysTick->VAL;
		sw_tic = sys_tic;
		hw_tic2 = SysTick->VAL;
	} while ( hw_tic2 > hw_tic1 );

	r = sw_tic;
	r *= 1000;
	r += (SYSTICK_RELOAD_VALUE - hw_tic2) / 8;	// Z�hler l�uft mit 8MHz darum /8 um ms zu bekommen

	return r;
}

/******************************************************************************************************
                                               COut
-------------------------------------------------------------------------------------------------------
Ausgabe von Debug-Info auf einer der seriellen Schnittstellen

Parameter:
   wie printf
******************************************************************************************************/
void COut( char *format, ... )
{
	va_list args;
	u32     len;

	va_start( args, format );      	// Argumentliste beginnt hinter diesem Parameter.
	vsprintf( DbgMsgStr, format, args );
	va_end( args );

	len = strlen( DbgMsgStr );

	SendData( DbgMsgStr, len, COM_CHANNEL_DEBUG );
	WaitTillSent();
}

/******************************************************************************************************
                                               DbgPrintf
-------------------------------------------------------------------------------------------------------
Ausgabe von Debug-Info auf einer der seriellen Schnittstellen mit CR abgeschlossen

Parameter:
   wie printf
******************************************************************************************************/
void DbgPrintf( char *format, ... )
{
	va_list args;
	u32     len;

	va_start( args, format );     	// Argumentliste beginnt hinter diesem Parameter.
	vsprintf( DbgMsgStr, format, args );
	va_end( args );

	len = strlen( DbgMsgStr );
	DbgMsgStr[len++] = 0x0D;
	DbgMsgStr[len++] = 0x0A;

	SendData( DbgMsgStr, len, COM_CHANNEL_DEBUG );
	WaitTillSent();
}

/******************************************************************************************************
                                                DbgHex
-------------------------------------------------------------------------------------------------------
Ausgabe von Hex-Dumps auf einer der seriellen Schnittstellen

Parameter:
   data:     Zeiger auf Daten
   len:      L�nge der Daten in Byte
******************************************************************************************************/
void DbgHex( void *data, u16 len )
{
	u16  n, max;
	char *sp;
	u8   *dp = data;

	while ( len > 0 )
	{
		max = len;
		if ( max > 16 ) max = 16;
		sp = DbgMsgStr;

		for ( n = 0; n < max; n++ )
		{
			sprintf( sp, "%02X ", *dp );
			sp += 3;
			dp++;
		}
		*sp++ = 0x0D;
		*sp++ = 0x0A;
		SendData( DbgMsgStr, strlen( DbgMsgStr ), COM_CHANNEL_DEBUG );
		len -= max;
	}
}

/******************************************************************************************************
                                               DbgPulse
-------------------------------------------------------------------------------------------------------

Parameter:
   PulsCnt:  ``
*******************************************************************************************************
******************************************************************************************************/
void DbgPulse( u16 PulsCnt ) 
{
	for (  ; PulsCnt > 0 ; PulsCnt-- )
	{
		GPIO_SetBits( DIO_TESTPIN1 );
		GPIO_ResetBits( DIO_TESTPIN1 );
	}
	DelayUsRaw( 1 );
}

/******************************************************************************************************
                                               GpioInit
-------------------------------------------------------------------------------------------------------
Initialisiert die IO Pins
******************************************************************************************************/
void GpioInit( void )
{
	u32 n;
	GPIO_InitTypeDef is;

	typedef struct InitConst_Strct_s
	{
		GPIO_TypeDef *Port;
		u16 Pin;
		GPIOMode_TypeDef  Mode;
		GPIOSpeed_TypeDef Speed;
	} InitConst_Strct;

	const InitConst_Strct ic[] =
	{
		{GPIOA, GPIO_Pin_0,  GPIO_Mode_AIN,			GPIO_Speed_2MHz }, 	// ADC_MESS_USB
		{GPIOA, GPIO_Pin_1,  GPIO_Mode_AIN,   		GPIO_Speed_2MHz }, 	// ADC_MESS_UIN1
		{GPIOA, GPIO_Pin_2,  GPIO_Mode_AIN, 		GPIO_Speed_2MHz }, 	// ADC_MESS_UIN2
		{GPIOA, GPIO_Pin_3,  GPIO_Mode_AIN,   		GPIO_Speed_2MHz }, 	// ADC_MESS_UMLICHT
		{GPIOA, GPIO_Pin_4,  GPIO_Mode_AF_PP, 		GPIO_Speed_2MHz }, 	// DAC_FLASHBRIGHT1
		{GPIOA, GPIO_Pin_5,  GPIO_Mode_AF_PP,  		GPIO_Speed_2MHz }, 	// DAC_FLASHBRIGHT2
		{GPIOA, GPIO_Pin_6,  GPIO_Mode_AIN,   		GPIO_Speed_2MHz }, 	// ADC_U_AKKU  	
		{GPIOA, GPIO_Pin_7,  GPIO_Mode_AIN,   		GPIO_Speed_2MHz }, 	// ADC_I_AKKU		
		{GPIOA, GPIO_Pin_8,  GPIO_Mode_IPU, 		GPIO_Speed_2MHz }, 	// DIN_nRTC_INT	(opt)
		{GPIOA, GPIO_Pin_9,  GPIO_Mode_AF_PP,  		GPIO_Speed_2MHz }, 	// RS232_TX		
		{GPIOA, GPIO_Pin_10, GPIO_Mode_IPD,     	GPIO_Speed_2MHz }, 	// RS232_RX		
		{GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DOUT_SENS_LOW	
		{GPIOA, GPIO_Pin_12, GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_DISP_ON	
		{GPIOA, GPIO_Pin_13, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz },	// SPEC_JTAG_SWDIO		
		{GPIOA, GPIO_Pin_14, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz },	// SPEC_JTAG_SWCLK
		{GPIOA, GPIO_Pin_15, GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DIO_NRSTCAM

		{GPIOB, GPIO_Pin_0,  GPIO_Mode_AIN,        	GPIO_Speed_2MHz }, 	// ADC_BQ_I		
		{GPIOB, GPIO_Pin_1,  GPIO_Mode_AIN, 		GPIO_Speed_2MHz },	// ADC_HW_VERSION	
		{GPIOB, GPIO_Pin_2,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// SPEC_BOOT1
		{GPIOB, GPIO_Pin_3,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DIO_IMX6_STM1
		{GPIOB, GPIO_Pin_4,  GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DIO_IMX6_STM2
		{GPIOB, GPIO_Pin_5,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_PIC_PWRON	
		{GPIOB, GPIO_Pin_6,  GPIO_Mode_AF_OD,     	GPIO_Speed_2MHz }, 	// I2C_SCL
		{GPIOB, GPIO_Pin_7,  GPIO_Mode_AF_OD,     	GPIO_Speed_2MHz }, 	// I2C_SDA
		{GPIOB, GPIO_Pin_8,  GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_PIEZO
		{GPIOB, GPIO_Pin_9,  GPIO_Mode_AF_PP, 		GPIO_Speed_2MHz }, 	// PWM_DISP
		{GPIOB, GPIO_Pin_10, GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_FLASHSW1	
		{GPIOB, GPIO_Pin_11, GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_FLASHSW2	
		{GPIOB, GPIO_Pin_12, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_TOUCH
		{GPIOB, GPIO_Pin_13, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nUSB3751_INT	
		{GPIOB, GPIO_Pin_14, GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DIO_ERWPIN
		{GPIOB, GPIO_Pin_15, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_IMX6_PWRGOOD

		{GPIOC, GPIO_Pin_0,  GPIO_Mode_AIN,        	GPIO_Speed_2MHz }, 	// ADC_AKKU_VERSION	
		{GPIOC, GPIO_Pin_1,  GPIO_Mode_AIN, 		GPIO_Speed_2MHz },	// ADC_AKKUTEMP1
		{GPIOC, GPIO_Pin_2,  GPIO_Mode_AIN,			GPIO_Speed_2MHz }, 	// ADC_AKKUTEMP2
		{GPIOC, GPIO_Pin_3,  GPIO_Mode_AIN,			GPIO_Speed_2MHz }, 	// ADC_KUEHLTEMP
		{GPIOC, GPIO_Pin_4,  GPIO_Mode_IPD,     	GPIO_Speed_2MHz }, 	// DIO_IMX6_STM3		
		{GPIOC, GPIO_Pin_5,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_3V3_ON			
		{GPIOC, GPIO_Pin_6,  GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_LED3_BLAU		
		{GPIOC, GPIO_Pin_7,  GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_LED4_ROT		
		{GPIOC, GPIO_Pin_8,  GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_LED4_GRN		
		{GPIOC, GPIO_Pin_9,  GPIO_Mode_AF_PP, 		GPIO_Speed_2MHz }, 	// PWM_LED4_BLAU		
		{GPIOC, GPIO_Pin_10, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_SENS		
		{GPIOC, GPIO_Pin_11, GPIO_Mode_IPD,     	GPIO_Speed_2MHz }, 	// DIO_IMX6_STM4		
		{GPIOC, GPIO_Pin_12, GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_BT_ON			
		{GPIOC, GPIO_Pin_13, GPIO_Mode_IPD,			GPIO_Speed_2MHz }, 	// NC
		{GPIOC, GPIO_Pin_14, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// SPEC_Q32_kHz
		{GPIOC, GPIO_Pin_15, GPIO_Mode_AF_OD,		GPIO_Speed_2MHz }, 	// SPEC_Q32_kHz

		{GPIOD, GPIO_Pin_0,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_NFC		
		{GPIOD, GPIO_Pin_1,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz },	// DIN_nSW_OC_SD_MEMS	
		{GPIOD, GPIO_Pin_2,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_WLAN_ON		
		{GPIOD, GPIO_Pin_3,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_TOUCH_ON		
		{GPIOD, GPIO_Pin_4,  GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DOUT_NFC_ON			
		{GPIOD, GPIO_Pin_5,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_SD_ON			
		{GPIOD, GPIO_Pin_6,  GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DOUT_MEMS_ON 		
		{GPIOD, GPIO_Pin_7,  GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DOUT_CAM_ON			
		{GPIOD, GPIO_Pin_8,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_FUNK		
		{GPIOD, GPIO_Pin_9,  GPIO_Mode_Out_PP,    	GPIO_Speed_2MHz }, 	// DOUT_SENS_HIGH		
		{GPIOD, GPIO_Pin_10, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_IMX6		
		{GPIOD, GPIO_Pin_11, GPIO_Mode_Out_PP,     	GPIO_Speed_2MHz }, 	// DOUT_5V_ON			
		{GPIOD, GPIO_Pin_12, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED2_GRN		
		{GPIOD, GPIO_Pin_13, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED2_BLAU		
		{GPIOD, GPIO_Pin_14, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED3_ROT		
		{GPIOD, GPIO_Pin_15, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED3_GRN		

		{GPIOE, GPIO_Pin_0,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_BQ_INT			
		{GPIOE, GPIO_Pin_1,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz },	// DIN_PIC_KEY_OUT		
		{GPIOE, GPIO_Pin_2,  GPIO_Mode_IPU,			GPIO_Speed_2MHz }, 	// DIN_nBQ_PGOOD
		{GPIOE, GPIO_Pin_3,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_BQ_STAT			
		{GPIOE, GPIO_Pin_4,  GPIO_Mode_Out_PP,    	GPIO_Speed_2MHz }, 	// DOUT_nBQ_CE			
		{GPIOE, GPIO_Pin_5,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_BQ_PSEL		
		{GPIOE, GPIO_Pin_6,  GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_MAX_RS232_CONN	
		{GPIOE, GPIO_Pin_7,  GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DOUT_WLED_ON
		{GPIOE, GPIO_Pin_8,  GPIO_Mode_IPU,     	GPIO_Speed_2MHz }, 	// DIN_nSW_OC_WLED
		{GPIOE, GPIO_Pin_9,  GPIO_Mode_AF_PP, 		GPIO_Speed_2MHz }, 	// PWM_LED1_ROT		
		{GPIOE, GPIO_Pin_10, GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DIO_TESTPIN1
		{GPIOE, GPIO_Pin_11, GPIO_Mode_AF_PP,     	GPIO_Speed_2MHz }, 	// PWM_LED1_GRN		
		{GPIOE, GPIO_Pin_12, GPIO_Mode_IN_FLOATING,	GPIO_Speed_2MHz }, 	// DIN_nTEMP_PWROFF
		{GPIOE, GPIO_Pin_13, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED1_BLAU
		{GPIOE, GPIO_Pin_14, GPIO_Mode_AF_PP,		GPIO_Speed_2MHz }, 	// PWM_LED2_ROT
		{GPIOE, GPIO_Pin_15, GPIO_Mode_Out_PP,		GPIO_Speed_2MHz }, 	// DIO_TESTPIN2
		{0,     0,           0,                    	 0              }  	// end marker
	};

	// IOs anhand der obigen Tabelle initialisieren
	n = 0;
	while ( ic[n].Pin != 0 )
	{
		is.GPIO_Pin = ic[n].Pin;
		is.GPIO_Mode = ic[n].Mode;
		is.GPIO_Speed = ic[n].Speed;
		GPIO_Init( ic[n].Port, &is );
		PinMode[n] = ic[n].Mode;
		n++;
	}

	// Nutzung von GPIOA-15, GPIOB-3 und GPIOB-4 zulassen
	GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );
	
	// Nutzung von Timer1-, Timer3- und Timer4-Pins als PWM-Ausgaenge
	GPIO_PinRemapConfig( GPIO_FullRemap_TIM1, ENABLE );
	GPIO_PinRemapConfig( GPIO_PartialRemap2_TIM2, ENABLE );
	GPIO_PinRemapConfig( GPIO_FullRemap_TIM3, ENABLE );
	GPIO_PinRemapConfig( GPIO_Remap_TIM4, ENABLE );

	// Grundeinstellungen der als Ausgaenge verwendeten Pins
	GPIO_ResetBits (DOUT_3V3_ON);	GPIO_ResetBits (DOUT_5V_ON);
	GPIO_ResetBits (DOUT_SENS_LOW);	GPIO_ResetBits (DOUT_SENS_HIGH);GPIO_ResetBits (DOUT_DISP_ON);
	GPIO_ResetBits (DOUT_BT_ON );	GPIO_ResetBits (DOUT_WLAN_ON);	GPIO_ResetBits (DOUT_TOUCH_ON);
	GPIO_ResetBits (DOUT_SD_ON);	GPIO_ResetBits (DOUT_NFC_ON);	GPIO_ResetBits (DOUT_MEMS_ON);
	GPIO_ResetBits (DOUT_CAM_ON);	GPIO_SetBits   (DOUT_nBQ_CE);	GPIO_SetBits   (DOUT_BQ_PSEL);
	GPIO_ResetBits (DIO_NRSTCAM);	GPIO_ResetBits (DIO_TESTPIN1);	GPIO_ResetBits (DIO_TESTPIN2);
	GPIO_ResetBits (DOUT_WLEDS_ON); GPIO_ResetBits (DIO_ERWPIN);	GPIO_ResetBits (DIO_IMX6_STM1);
	GPIO_ResetBits (DIO_IMX6_STM2);

	TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_InActive);
	TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_InActive);

}


/******************************************************************************************************
                                           RCC_Configuration_On
-------------------------------------------------------------------------------------------------------
Configures system clocks for on mode
******************************************************************************************************/
void RCC_Configuration_On( void )
{
	// Enable HSE (8 MHz) and wait till HSE is ready
	RCC_HSEConfig( RCC_HSE_ON );
	RCC_WaitForHSEStartUp();

	// Enable Prefetch Buffer and set Flash latency to two wait states
	FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable );
	FLASH_SetLatency( FLASH_Latency_2 );

	// HCLK = PCLK1 = PCLK2 = SYSCLK = 8MHz
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );
	RCC_PCLK1Config( RCC_HCLK_Div1 );
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	// PLLCLK = HSE (keine PLL)
	RCC_PLLCmd( DISABLE );

	// Select HSE as system clock source and wait for HSE switch
	RCC_SYSCLKConfig( RCC_SYSCLKSource_HSE );
	while( RCC_GetSYSCLKSource() != 0x04 ) ;

	// Intern low speed oscillator off
	RCC_LSICmd( DISABLE );
	
	// Configure ADC-Clock = 1 MHz
	RCC_ADCCLKConfig( RCC_PCLK2_Div8 );
	
	// DMA clock enable
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_DMA1, ENABLE );

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2		| RCC_APB1Periph_TIM3	| RCC_APB1Periph_TIM4	| 
							RCC_APB1Periph_WWDG		| RCC_APB1Periph_I2C1	| RCC_APB1Periph_BKP	|
							RCC_APB1Periph_PWR		| RCC_APB1Periph_DAC	, ENABLE );

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO		| RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB 	| 
                            RCC_APB2Periph_GPIOC	| RCC_APB2Periph_GPIOD  | RCC_APB2Periph_GPIOE	|
                            RCC_APB2Periph_ADC1		| RCC_APB2Periph_TIM1	| RCC_APB2Periph_USART1	,
							ENABLE );

	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

	// Output Sysclock
	// RCC_MCOConfig( RCC_MCO_SYSCLK )
}


/******************************************************************************************************
                                           RCC_Configuration_Off
-------------------------------------------------------------------------------------------------------
Configures system clocks for on mode
******************************************************************************************************/
void RCC_Configuration_Off( void )
{
	// Disable HSE (8 MHz) and wait till HSE is ready
	RCC_HSEConfig( RCC_HSE_OFF );

	// Enable Prefetch Buffer and set Flash latency to two wait states
	FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable );
	FLASH_SetLatency( FLASH_Latency_2 );

	// HCLK = PCLK1 / 125 kHz; PCLK2 = SYSCLK = 500 kHz
	RCC_HCLKConfig( RCC_SYSCLK_Div64 );
	RCC_PCLK1Config( RCC_HCLK_Div16 );
	RCC_PCLK2Config( RCC_HCLK_Div16 );

	// PLLCLK = HSI (keine PLL)
	RCC_PLLCmd( DISABLE );

	RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI );

	// Intern low speed oscillator off
	RCC_LSICmd( DISABLE );

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_WWDG		| RCC_APB1Periph_BKP	| RCC_APB1Periph_PWR, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB 	| RCC_APB2Periph_GPIOE, ENABLE );

}


/******************************************************************************************************
                                           RTC_Configuration
-------------------------------------------------------------------------------------------------------
Configures RTC
******************************************************************************************************/
void RTC_Configuration( void )
{
	u32 IntReg = BKP_ReadBackupRegister(BKP_DR1);
	u32 Timeout;

	if ( IntReg != 0xA5A5 )
	{
		PWR_BackupAccessCmd(ENABLE);
		BKP_DeInit();
		RCC_LSEConfig( RCC_LSE_ON );
		Timeout = 0;
		while ( (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (Timeout++ < MAXCLKTIME ) );
		if (Timeout < MAXCLKTIME)
		{
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
			RCC_RTCCLKCmd(ENABLE);
			RTC_WaitForLastTask();
			RTC_WaitForSynchro();
			RTC_WaitForLastTask();
			RTC_SetPrescaler(32768);
			RTC_WaitForLastTask();
			BKP_TamperPinCmd(DISABLE);
			BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
			BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		}
		PWR_BackupAccessCmd(DISABLE);
	}
	else
	{
		RCC_LSEConfig( RCC_LSE_ON );
		Timeout = 0;
		while ( (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (Timeout++ < MAXCLKTIME ) );
		if (Timeout < MAXCLKTIME)
		{
			RTC_WaitForSynchro();
			RTC_WaitForLastTask();
		}
	}
}

/******************************************************************************************************
                                           RTC_SetTime
-------------------------------------------------------------------------------------------------------
Zeit an RTC uebergeben
******************************************************************************************************/
void RTC_SetTime( u32 Seconds )
{
	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	RTC_EnterConfigMode();
	RTC_WaitForLastTask();
	RTC_SetCounter(Seconds);//(IntTime) (tim);
	RTC_WaitForLastTask();
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
}

/******************************************************************************************************
                                           RTC_GetTime
-------------------------------------------------------------------------------------------------------
Zeit aus RTC auslesen
******************************************************************************************************/
void RTC_GetTime( u32 *Seconds )
{
	u32 IntCounter;

	PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
	IntCounter = RTC_GetCounter();
	RTC_WaitForLastTask();
	IntCounter = RTC_GetCounter();
	RTC_WaitForLastTask();
	PWR_BackupAccessCmd(DISABLE);
	*Seconds = IntCounter;
}

/******************************************************************************************************
                                          EXTI_Configuration
-------------------------------------------------------------------------------------------------------
Configure the external interrupt controller.
******************************************************************************************************/
void EXTI_Configuration( void )
{
	EXTI_InitTypeDef 	 exti;
	NVIC_InitTypeDef     nis;

	EXTI_ClearITPendingBit(EXTI_Line0);

	// Define external Interrupts
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
	exti.EXTI_Line = EXTI_Line0;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;
	exti.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti);

	nis.NVIC_IRQChannel = EXTI0_IRQn;
	nis.NVIC_IRQChannelPreemptionPriority = 15;
	nis.NVIC_IRQChannelSubPriority = 0;
	nis.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nis );

}

/******************************************************************************************************
                                          NVIC_Configuration
-------------------------------------------------------------------------------------------------------
Configure the nested vectored interrupt and external interrupt controller.
******************************************************************************************************/
void NVIC_Configuration( void ) 
{
	extern unsigned long _sisr_vector;  		// defined in linker script
	NVIC_InitTypeDef     nis;

	NVIC_SetVectorTable( NVIC_VectTab_FLASH, (u32)&_sisr_vector );
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	// Enable DMA1 channel4 IRQ Channel for RS232 TX
	nis.NVIC_IRQChannel = DMA1_Channel4_IRQn;    
	nis.NVIC_IRQChannelPreemptionPriority = 15;
	nis.NVIC_IRQChannelSubPriority = 0;
	nis.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nis );
	
	// Enable DMA1 channel5 IRQ Channel for RS232 RX
	nis.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	nis.NVIC_IRQChannelPreemptionPriority = 15;
	nis.NVIC_IRQChannelSubPriority = 0;
	nis.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nis );
	
	// Enable USART1 IRQ Channel
	nis.NVIC_IRQChannel = USART1_IRQn;           // COM_CHANNEL_RS RX line idle or error
	nis.NVIC_IRQChannelPreemptionPriority = 15;
	nis.NVIC_IRQChannelSubPriority = 0;
	nis.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nis );

	// ADC1 IRQ for voltage measurement
	nis.NVIC_IRQChannel = ADC1_IRQn;             // ADC 1 CHANNEL for inklinometer, ...
	nis.NVIC_IRQChannelPreemptionPriority = 15;
	nis.NVIC_IRQChannelSubPriority = 0;
	nis.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &nis );

}


/******************************************************************************************************
                                              ErrorBlink
-------------------------------------------------------------------------------------------------------
Rote Power-LED morst die 10er Stelle des Errorcodes, Rote Laser-LED morst 1er Stelle des Fehlercodes.

Parameter:
   errcode:  siehe #defines ERR_BLINK_xxx
   RunTasks: wenn TRUE werden noch gewisse Tasks ausgef�hrt
******************************************************************************************************/
void ErrorBlink( u16 errcode, bool RunTasks ) 
{
/*
	u32 next_blink;
	u32 next_morse;
	s32 n;
	u16 err10, err1;
	u32 tic = 0;
	u32 hw_tic, hw_tics;

	SetLedsErr( LED_ALL, OFF );
	err10 = errcode / 10;
	err1 = errcode % 10;

	next_blink = 0;
	next_morse = 0;

	SysTick->LOAD = SYSTICK_RELOAD_VALUE;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000003;

	n = 0;
	for (  ;  ; )
	{
		// 1ms warten ohne Verwendung von Delay-Routinen die auf IRQs basieren
		hw_tics = 0;
		do
		{
			hw_tic = SysTick->VAL;
			while ( SysTick->VAL == hw_tic ) ;
			hw_tics++;
		} while ( hw_tics < 1500 );
		tic++;   // ms tic weiterschalten

		if ( (tic - next_morse) >= 200 )
		{
			next_morse = tic;

			if ( 1 == (n & 0x01) )
			{ SetLedsErr( LED_ALLRED, OFF ); }
			else
			{
				if ( (n>>1) < err10 )
					SetLedsErr( LED_PRED, ON );
				if ( (n>>1) < err1 )
					SetLedsErr( LED_LRED, ON );
			}
			n++;
			if ( n >= (max( err1, err10 )+3) * 2 )
				n = 0;
		}
		if ( RunTasks == TRUE )
			RxTask();
	}
	*/
}


/*******************************************************************************
* Function Name  : NVIC_DeInit
* Description    : Deinitializes the NVIC peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_DeInit( void ) 
{
	u32 index = 0;

	NVIC->ICER[0] = 0xFFFFFFFF;
	NVIC->ICER[1] = 0x0FFFFFFF;
	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICPR[1] = 0x0FFFFFFF;

	for( index = 0; index < 0x0F; index++ )
		NVIC->IP[index] = 0x00000000;
}

/*******************************************************************************
* Function Name  : NVIC_SCBDeInit
* Description    : Deinitializes the SCB peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_SCBDeInit( void ) 
{
  	#define AIRCR_VECTKEY_MASK    ( (u32)0x05FA0000 )
	u32 index = 0x00;

	SCB->ICSR = 0x0A000000;
	SCB->VTOR = 0x00000000;
	SCB->AIRCR = AIRCR_VECTKEY_MASK;
	SCB->SCR = 0x00000000;
	SCB->CCR = 0x00000000;
	for( index = 0; index < 0x03; index++ )
		SCB->SHP[index] = 0;
	SCB->SHCSR = 0x00000000;
	SCB->CFSR = 0xFFFFFFFF;
	SCB->HFSR = 0xFFFFFFFF;
	SCB->DFSR = 0xFFFFFFFF;
}


/*******************************************************************************
* Function Name  : ADC_Configuration
* Description    : Initialisiert und Kalibriert den AD-Wandler fuer die Messung
* 				   von Spannungen, Stroemen, Umgebungshelligkeit und checkt vor-
* 				   her einmalig die Versionsstaende
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Configuration ( void )
{
	ADC_InitTypeDef ADC_Conf = {0,0,0,0,0,0};

	// Disable ADC first
	ADC_Cmd(ADC1, DISABLE);

	ADC_Conf.ADC_Mode = ADC_Mode_Independent;
	ADC_Conf.ADC_ScanConvMode = ENABLE;
	ADC_Conf.ADC_ContinuousConvMode = DISABLE;
	ADC_Conf.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Conf.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Conf.ADC_NbrOfChannel = 0;
	ADC_Init(ADC1, &ADC_Conf);

	// Initialisierung: HW-Versionen und Akkutemperaturen messen
	ADC_InjectedSequencerLengthConfig(ADC1, 4);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_9,  1, ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 3, ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_12, 4, ADC_SampleTime_239Cycles5);

	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);

	// interne Temperaturmessung wird nicht mehr verwendet
	// ADC_TempSensorVrefintCmd(ENABLE);

	// enable ADC interrupt and ADC1
	ADC_Cmd(ADC1, ENABLE);
	DelayUsRaw( 1000 );

	// Enable ADC1 reset calibaration register
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1));

	// Start ADC1 calibaration
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));

	// start first ADC measurement by software
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
	ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
}

/*******************************************************************************
* Function Name  : Interrupt-Funktion fuer A/D-Wandler
* Description    : wird aufgerufen, wenn eine Wandlung (4 Kan�le) durch ist.
* 				   Bereitet die naechste Messung vor und startet sie.
* Input          : None
* Output         : None
* Return         : None
*
* Liste der A/D-Werte (jeweils 12 Bit -> Wertebereich: 0...4095):
* - ADC1_IN0:  Spannung am USB-Eingang (max. Spannung: 10,08V)
* - ADC1_IN1:  Spannung am Hohlstecker (max. Spannung: 24,24V)
* - ADC1_IN2:  Spannung an der ODU-Buchse  (max. Spannung: 24,24V)
* - ADC1_IN3:  Spannung am Umlichtsensor
* - ADC1_IN6:  Spannung am Akku (max. Spannung: 5V)
* - ADC1_IN7:  Strom am Akku (bidirektional) (Strom: -5A...5A)
* - ADC1_IN8:  Eingangsstrom ext. Versorgung (max. Strom: 4,42A -> 1V -> 1638 St.)
* - ADC1_IN9:  Motherboard-Hardwareversion (per Widerstandsteiler eingestellt)
* - ADC1_IN10: Akku-Version (per Widerstandsteiler eingestellt)
* - ADC1_IN11: Akkupack1-Temperatur (Wertzuordnung gem�� Datenblatt des NTC B57861-S103-F40)
* - ADC1_IN12: Akkupack2-Temperatur (Wertzuordnung gem�� Datenblatt des NTC B57861-S103-F40)
* - ADC1_IN13: K�hlk�rper-Temperatur (Wertzuordnung gem�� Datenblatt des NTC NCP18XH103F03RB)
*******************************************************************************/
void ADC1_2_IRQHandler( void )
{
	static u8 LoopCount = 0, AkkuCount = 0, InputCount = 0, ValueLt = 0;

	//DbgPulse (1);
	ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

	switch (LoopCount)
	{
		case 0:
			HwInfoData.hws_runf     = 1 + (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1) >> 7);
			HwInfoData.hws_akku     = 1 + (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2) >> 7);
			AnalogVals.TempAkku1Raw = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3);
			AnalogVals.TempAkku2Raw = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_4);

			ADC_InjectedSequencerLengthConfig(ADC1, 4);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_13,4, ADC_SampleTime_239Cycles5);

			LoopCount++;
		break;

		case 1: case 2: case 3: case 6: case 7: case 8:	case 11: case 12: case 13: case 16: case 17: case 18:
			AmbVolt[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
			AkkVolt[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
			AkkCurr[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3);
			KuehlTemp[AkkuCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_4);

			AkkuCount++;
			LoopCount++;
		break;

		case 4:	case 9: case 14: case 19:
			AmbVolt[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
			AkkVolt[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
			AkkCurr[AkkuCount]   = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3);
			KuehlTemp[AkkuCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_4);

			ADC_InjectedSequencerLengthConfig(ADC1, 4);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 4, ADC_SampleTime_239Cycles5);

			AkkuCount++;
			LoopCount++;
		break;

		case 5: case 10: case 15:
			UsbVolt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
			In1Volt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
			In2Volt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3);
			InpCurr[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_4);

			ADC_InjectedSequencerLengthConfig(ADC1, 4);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_13,4, ADC_SampleTime_239Cycles5);

			InputCount++;
			LoopCount++;
		break;

		case 20:
			UsbVolt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
			In1Volt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_2);
			In2Volt[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_3);
			InpCurr[InputCount] = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_4);

			ADC_InjectedSequencerLengthConfig(ADC1, 4);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_9,  1, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_11, 3, ADC_SampleTime_239Cycles5);
			ADC_InjectedChannelConfig(ADC1, ADC_Channel_12, 4, ADC_SampleTime_239Cycles5);

			AkkVoltLt[ValueLt] = 0;
			AkkCurrLt[ValueLt] = 0;

			for (AkkuCount=0; AkkuCount<16; AkkuCount++)
			{
				AkkVoltLt[ValueLt] += AkkVolt[AkkuCount];
				AkkCurrLt[ValueLt] += AkkCurr[AkkuCount];
			}
			AkkVoltLt[ValueLt] >>= 4;
			AkkCurrLt[ValueLt] >>= 4;

			ValueLt = (ValueLt + 1) & 0x0F;
			AkkuCount = 0;
			InputCount = 0;
			LoopCount = 0;

		break;
	}

	ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
	//DbgPulse (2);
}


/*******************************************************************************
* Function Name  : PWM_Configuration
* Description    : Initialisiert die Timer / PWMs f�r die Benutzung als LED-,
				   Blitz-, Piezo- und Displaybeleuchtungs-Ausg�nge 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_Configuration ( void )
{
	TIM_TimeBaseInitTypeDef TimeBaseInit = {0,0,0,0,0};
	TIM_OCInitTypeDef OCInit = {0,0,0,0,0,0,0,0};
	
	// alle PWMs: 1 Periode hat 1000 Ticks; Aufwaertszaehler; Quarzfrequenz 8 MHz
	TimeBaseInit.TIM_Period = 1000;
	TimeBaseInit.TIM_Prescaler = 0;
	TimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	TimeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	TimeBaseInit.TIM_RepetitionCounter = 0;

	// LEDs werden an 2 kHz-PWM betrieben
	TimeBaseInit.TIM_Prescaler = 3;
	TIM_TimeBaseInit ( TIM1, &TimeBaseInit );
	TIM_TimeBaseInit ( TIM3, &TimeBaseInit );
	TIM_TimeBaseInit ( TIM4, &TimeBaseInit );

	// Display-LEDs werden an 125 Hz-PWM betrieben (neu: 1000 Hz)
	//TimeBaseInit.TIM_Prescaler = 64;
	TimeBaseInit.TIM_Prescaler = 7;
	TIM_TimeBaseInit ( TIM17, &TimeBaseInit );

	// Piezo- und WLED-Basisfrequenz = 1 MHz; uebrige Timerparameter werden spaeter variiert
	TimeBaseInit.TIM_Prescaler = 7;
	TIM_TimeBaseInit ( TIM2, &TimeBaseInit );
	TIM_TimeBaseInit ( TIM16, &TimeBaseInit );

	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM16, ENABLE);
	TIM_Cmd(TIM17, ENABLE);

	OCInit.TIM_OCMode = TIM_OCMode_PWM1;
	OCInit.TIM_Pulse = 0;
	OCInit.TIM_OutputState = TIM_OutputState_Enable;
	OCInit.TIM_OCPolarity = TIM_OCPolarity_High;
	OCInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init (TIM1, &OCInit );
	TIM_OC2Init (TIM1, &OCInit );
	TIM_OC3Init (TIM1, &OCInit );
	TIM_OC4Init (TIM1, &OCInit );
	TIM_OC3Init (TIM2, &OCInit );
	TIM_OC4Init (TIM2, &OCInit );
	TIM_OC1Init (TIM3, &OCInit );
	TIM_OC2Init (TIM3, &OCInit );
	TIM_OC3Init (TIM3, &OCInit );
	TIM_OC4Init (TIM3, &OCInit );
	TIM_OC1Init (TIM4, &OCInit );
	TIM_OC2Init (TIM4, &OCInit );
	TIM_OC3Init (TIM4, &OCInit );
	TIM_OC4Init (TIM4, &OCInit );
	TIM_OC1Init (TIM16, &OCInit );
	TIM_OC1Init (TIM17, &OCInit );

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM16, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM16, ENABLE);
	TIM_CtrlPWMOutputs(TIM17, ENABLE);
}

/*******************************************************************************
* Function Name  : DAC_Configuration
* Description    : Initialisiert den DA-Wandler so, dass Spannungen zwischen
* 				   0V und 2,5V ausgegeben werden k�nnen.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_Configuration ( void )
{
	DAC_InitTypeDef DAC_Conf;

	DAC_Conf.DAC_Trigger = DAC_Trigger_None;
	DAC_Conf.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_Conf.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init (DAC_Channel_1, &DAC_Conf);
	DAC_Init (DAC_Channel_2, &DAC_Conf);
	DAC_Cmd (DAC_Channel_1, ENABLE);
	DAC_Cmd (DAC_Channel_2, ENABLE);
	DAC_SetDualChannelData(DAC_Align_12b_R, 0, 0);
}

/******************************************************************************************************
                                           InitLedState
-------------------------------------------------------------------------------------------------------

******************************************************************************************************/
void InitLedState( void )
{
	u16 i;

	TIM_SetCompare1( TIM1, 0 );
	TIM_SetCompare2( TIM1, 0 );
	TIM_SetCompare3( TIM1, 0 );
	TIM_SetCompare4( TIM1, 0 );
	TIM_SetCompare3( TIM2, 0 );
	TIM_SetCompare4( TIM2, 0 );
	TIM_SetCompare1( TIM3, 0 );
	TIM_SetCompare2( TIM3, 0 );
	TIM_SetCompare3( TIM3, 0 );
	TIM_SetCompare4( TIM3, 0 );
	TIM_SetCompare1( TIM4, 0 );
	TIM_SetCompare2( TIM4, 0 );
	TIM_SetCompare3( TIM4, 0 );
	TIM_SetCompare4( TIM4, 0 );

	for (i=0; i<12; i++)
	{
		RgbLedState.ComponentState[i] = LED_COMP_OFF;
		RgbLedState.ActBrightComp[i] = 0;
		RgbLedState.BrightComp[i] = 0;
		RgbLedState.FinalOffTime[i] = 0;
		RgbLedState.FirstTime[i] = 0;
		RgbLedState.LastTime[i] = 0;
		RgbLedState.PeriodicOffTime[i] = 0;
		RgbLedState.PeriodicOnTime[i] = 0;
	}
}

/******************************************************************************************************
                                          SetLedBlinking
-------------------------------------------------------------------------------------------------------
Parameter:

******************************************************************************************************/
s8 SetLedState( u16 ChangeMask, u16 BrightRed, u16 BrightGreen, u16 BrightBlue,
		        u32 FinalOffTime, u32 PeriodicOnTime, u32 PeriodicOffTime )
{
	u16 i;
	u16 mask = 0x0001;
	u8 SomethingDone = 0;

	for (i=0; i<12; i++)
	{
		if ( (ChangeMask & mask) > 0)
		{
			if ((i%3) == 0) RgbLedState.BrightComp[i] = BrightRed;
			if ((i%3) == 1) RgbLedState.BrightComp[i] = BrightGreen;
			if ((i%3) == 2) RgbLedState.BrightComp[i] = BrightBlue;
			RgbLedState.FirstTime[i] = sys_tic;
			RgbLedState.LastTime[i] = sys_tic;
			RgbLedState.FinalOffTime[i] = FinalOffTime;
			RgbLedState.PeriodicOnTime[i] = PeriodicOnTime;
			RgbLedState.PeriodicOffTime[i] = PeriodicOffTime;
			if (PeriodicOffTime == 0) RgbLedState.ComponentState[i] = LED_COMP_ON;
			else RgbLedState.ComponentState[i] = LED_COMP_CHANGED_TO_BLINK;
			SomethingDone++;
		}

		if ((i%3) == 2) { mask <<= 2; }
		else { mask <<= 1; }
	}

	if (SomethingDone)
	{
		CheckLedState();
		return 0;
	}
	else { return -1; }
}

/******************************************************************************************************
                                           CheckLedBlinking
-------------------------------------------------------------------------------------------------------

******************************************************************************************************/
void CheckLedState( void )
{
	u16 i=0;
	u8 LedBlink = 0;
	//static u8 CompStateOld[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	static u32 LastTimeOff = 0;

	if ( sys_tic >= WLed1OffTime )
	{
		TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_InActive);
		//TIM_SetCompare3( TIM2, 0);
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	}

	if ( sys_tic >= WLed2OffTime )
	{
		TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_InActive);
		//TIM_SetCompare4( TIM2, 0);
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);
	}

	if ( sys_tic >= BeepOffTime ) TIM_SetCompare1( TIM16, 0);

	for (i=0; i<12; i++)
	{
		if ( (sys_tic >= ( RgbLedState.FirstTime[i] + RgbLedState.FinalOffTime[i] ) )
		   && (RgbLedState.FinalOffTime[i] < 0x80000000) )
		{
			RgbLedState.ComponentState[i] = LED_COMP_OFF;
			RgbLedState.LastTime[i] = sys_tic;
			RgbLedState.ActBrightComp[i] = 0;
			RgbLedState.BrightComp[i] = 0;
			RgbLedState.FinalOffTime[i] = 0;
			RgbLedState.PeriodicOffTime[i] = 0;
			RgbLedState.PeriodicOnTime[i] = 0;
		}
		else
		{
			if (RgbLedState.ComponentState[i] == LED_COMP_BLINK_ON)
			{
				if (sys_tic >= ( RgbLedState.LastTime[i] + RgbLedState.PeriodicOnTime[i] ) )
				{
					RgbLedState.ComponentState[i] = LED_COMP_BLINK_OFF;
					RgbLedState.LastTime[i] = sys_tic;
				}
				LedBlink++;
			}

			if (RgbLedState.ComponentState[i] == LED_COMP_BLINK_OFF)
			{
				if (sys_tic >= ( RgbLedState.LastTime[i] + RgbLedState.PeriodicOffTime[i] ) )
				{
					RgbLedState.ComponentState[i] = LED_COMP_BLINK_ON;
					RgbLedState.LastTime[i] = sys_tic;
					LastTimeOff = RgbLedState.LastTime[i];
				}
				LedBlink++;
			}
		}
	}
	
	if (LedBlink == 0)
	{
		for (i=0; i<12; i++)
		{
			if (RgbLedState.ComponentState[i] == LED_COMP_CHANGED_TO_BLINK)
			{
				RgbLedState.ComponentState[i] = LED_COMP_BLINK_ON;
				RgbLedState.LastTime[i] = sys_tic;
				LastTimeOff = 0;
			}
		}
	}
	else
	{
		for (i=0; i<12; i++)
		{
			if (RgbLedState.ComponentState[i] == LED_COMP_CHANGED_TO_BLINK)
			{
				if (LastTimeOff != 0)
				{
					RgbLedState.ComponentState[i] = LED_COMP_BLINK_ON;
					RgbLedState.LastTime[i] = LastTimeOff;
				}
			}
		}
	}

	for (i=0; i<12; i++)
	{
		//if (CompStateOld[i] != RgbLedState.ComponentState[i])
		{
			if  ( (RgbLedState.ComponentState[i] == LED_COMP_ON)
			   || (RgbLedState.ComponentState[i] == LED_COMP_BLINK_ON) )
			{
				switch (i)
				{
					case 0:	 TIM_SetCompare1( TIM1, RgbLedState.ActBrightComp[i] ); break;
					case 1:  TIM_SetCompare2( TIM1, RgbLedState.ActBrightComp[i] ); break;
					case 2:  TIM_SetCompare3( TIM1, RgbLedState.ActBrightComp[i] ); break;
					case 3:  TIM_SetCompare4( TIM1, RgbLedState.ActBrightComp[i] ); break;
					case 4:  TIM_SetCompare1( TIM4, RgbLedState.ActBrightComp[i] ); break;
					case 5:	 TIM_SetCompare2( TIM4, RgbLedState.ActBrightComp[i] ); break;
					case 6:  TIM_SetCompare3( TIM4, RgbLedState.ActBrightComp[i] ); break;
					case 7:  TIM_SetCompare4( TIM4, RgbLedState.ActBrightComp[i] ); break;
					case 8:  TIM_SetCompare1( TIM3, RgbLedState.ActBrightComp[i] ); break;
					case 9:  TIM_SetCompare2( TIM3, RgbLedState.ActBrightComp[i] ); break;
					case 10: TIM_SetCompare3( TIM3, RgbLedState.ActBrightComp[i] ); break;
					case 11: TIM_SetCompare4( TIM3, RgbLedState.ActBrightComp[i] ); break;
				}
			}

			if  ( (RgbLedState.ComponentState[i] == LED_COMP_OFF)
			   || (RgbLedState.ComponentState[i] == LED_COMP_BLINK_OFF) )
			{
				switch (i)
				{
					case 0:	 TIM_SetCompare1( TIM1, 0 ); break;
					case 1:  TIM_SetCompare2( TIM1, 0 ); break;
					case 2:  TIM_SetCompare3( TIM1, 0 ); break;
					case 3:  TIM_SetCompare4( TIM1, 0 ); break;
					case 4:  TIM_SetCompare1( TIM4, 0 ); break;
					case 5:	 TIM_SetCompare2( TIM4, 0 ); break;
					case 6:  TIM_SetCompare3( TIM4, 0 ); break;
					case 7:  TIM_SetCompare4( TIM4, 0 ); break;
					case 8:  TIM_SetCompare1( TIM3, 0 ); break;
					case 9:  TIM_SetCompare2( TIM3, 0 ); break;
					case 10: TIM_SetCompare3( TIM3, 0 ); break;
					case 11: TIM_SetCompare4( TIM3, 0 ); break;
				}
			}
		}
		//CompStateOld[i] = RgbLedState.ComponentState[i];
	}
}

/******************************************************************************************************
                                          SetFlashLedMode
-------------------------------------------------------------------------------------------------------
Parameter:

******************************************************************************************************/
void SetFlashLedMode ( void )
{
	TIM_OCInitTypeDef OCInit = {0,0,0,0,0,0,0,0};

	DAC_Configuration ();
	OCInit.TIM_OCMode = TIM_OCMode_PWM1;
	OCInit.TIM_Pulse = 0;
	OCInit.TIM_OutputState = TIM_OutputState_Enable;
	OCInit.TIM_OCPolarity = TIM_OCPolarity_High;
	OCInit.TIM_OCIdleState = TIM_OCIdleState_Reset;

	WLed1OffTime = 0xFFFFFFFF;
	WLed2OffTime = 0xFFFFFFFF;

	if (FlashLedState.BrightComp[0] > 4095)
		FlashLedState.BrightComp[0] = 4095;

	if (FlashLedState.BrightComp[1] > 4095)
		FlashLedState.BrightComp[1] = 4095;

	// LED 1 off
//	if ( (FlashLedState.ModeMask & 0x0007) == 0)
	{
		TIM_OC3Init (TIM2, &OCInit);
		TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_InActive);
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	}

	// LED 2 OFF
//	if ( (FlashLedState.ModeMask & 0x0700) == 0)
	{
		TIM_OC4Init (TIM2, &OCInit);
		TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_InActive);
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);
	}

	// LED1 als Taschenlampe
	if ( (FlashLedState.ModeMask & LED_WHITELED1_TORCH) != 0)
	{
		TIM_OC3Init (TIM2, &OCInit);
		TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_Active);
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
	}

	// LED2 als Taschenlampe
	if ( (FlashLedState.ModeMask & LED_WHITELED2_TORCH) != 0)
	{
		TIM_OC4Init (TIM2, &OCInit);
		if ( (FlashLedState.ModeMask & LED_WHITELED1_TORCH) != 0) DelayMs (3);
		TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_Active);
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);
	}

	// LED1 oder/und LED2 als Taschenlampe
	if ( (FlashLedState.ModeMask & LED_WHITELED1_TORCH)
	  || (FlashLedState.ModeMask & LED_WHITELED2_TORCH) )
	{
		u16 Bright12, n;

		if (FlashLedState.BrightComp[0] > FlashLedState.BrightComp[1])
		{ Bright12 = FlashLedState.BrightComp[0]; }
		else
		{ Bright12 = FlashLedState.BrightComp[1]; }

		for (n=0; n<=Bright12; n++)
		{
			if ( (FlashLedState.ModeMask & LED_WHITELED1_TORCH) && (n <= FlashLedState.BrightComp[0]) )
				DAC_SetChannel1Data(DAC_Align_12b_R, n);
			if ( (FlashLedState.ModeMask & LED_WHITELED2_TORCH) && (n <= FlashLedState.BrightComp[1]) )
				DAC_SetChannel2Data(DAC_Align_12b_R, n);
			DelayUsRaw (20);
		}
	}

	// LED1 oder/und LED2 als Stroboskoplampe
	if ( ( (FlashLedState.ModeMask & LED_WHITELED1_STROBE) != 0)
	  || ( (FlashLedState.ModeMask & LED_WHITELED2_STROBE) != 0) )
	{
		TIM_OC3Init (TIM2, &OCInit );
		TIM_OC4Init (TIM2, &OCInit );
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);
		TIM_PrescalerConfig(TIM2, 7, TIM_PSCReloadMode_Immediate);
		TIM_SetCounter (TIM2, 0);

		if ( (FlashLedState.ModeMask & LED_WHITELED1_STROBE) != 0)
		{
			DAC_SetChannel1Data(DAC_Align_12b_R, FlashLedState.BrightComp[0]);
			TIM_SetAutoreload(TIM2, FlashLedState.PeriodicTime-1);
			TIM_SetCompare3(TIM2, FlashLedState.OnTime);
		}

		if ( (FlashLedState.ModeMask & LED_WHITELED2_STROBE) != 0)
		{
			DAC_SetChannel2Data(DAC_Align_12b_R, FlashLedState.BrightComp[1]);
			TIM_SetAutoreload(TIM2, FlashLedState.PeriodicTime-1);
			TIM_SetCompare4( TIM2, FlashLedState.OnTime);
		}
	}

	// LED1 oder/und LED2 als Blitz
	if ( ( (FlashLedState.ModeMask & LED_WHITELED1_FLASH) != 0)
	  || ( (FlashLedState.ModeMask & LED_WHITELED2_FLASH) != 0)	)
	{
		TIM_OC3Init (TIM2, &OCInit );
		TIM_OC4Init (TIM2, &OCInit );
		TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_InActive);
		TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_InActive);
		if (FlashLedState.OnTime < 10) FlashLedState.OnTime = 10;
		DAC_SetChannel1Data(DAC_Align_12b_R, 0);
		DAC_SetChannel2Data(DAC_Align_12b_R, 0);

		if ( (FlashLedState.ModeMask & LED_WHITELED1_FLASH) != 0)
		{
			WLed1OffTime = sys_tic + FlashLedState.OnTime;
			DAC_SetChannel1Data(DAC_Align_12b_R, FlashLedState.BrightComp[0]);
			TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_Active);
		}

		if ( (FlashLedState.ModeMask & LED_WHITELED2_FLASH) != 0)
		{
			if ( (FlashLedState.ModeMask & LED_WHITELED1_FLASH) != 0) DelayMs (3);
			WLed2OffTime = sys_tic + FlashLedState.OnTime;
			DAC_SetChannel2Data(DAC_Align_12b_R, FlashLedState.BrightComp[1]);
			TIM_ForcedOC4Config(TIM2, TIM_ForcedAction_Active);
		}
	}
}

/******************************************************************************************************
                                          Beep
-------------------------------------------------------------------------------------------------------
Parameter:

******************************************************************************************************/
void Beep( u16 Frequency, u16 Time )
{
	u16 IntFreq;

	BeepOffTime = sys_tic + (u32)Time;
	IntFreq = (u16) ( ( (1000.0 / (float)Frequency) ) * 1000.0 );
	TIM_SetAutoreload(TIM16, IntFreq);
	TIM_SetCompare1( TIM16, IntFreq / 2);
}


/******************************************************************************************************
                                          SetDisplayBrightness
-------------------------------------------------------------------------------------------------------
Parameter:

******************************************************************************************************/
void SetDispLedBrightness( u8 AutoManu, u16 DispBrightness, u16 LedBrightness )
{
	u16 i;

	if (AutoManu > 0)
	{
		AnalogVals.Ambient_AD = 0;
		for (i=0; i<16; i++) AnalogVals.Ambient_AD += AmbVolt[i];

		if (AutoManu & AUTODISP)
		{
			DispBrightness = (AnalogVals.Ambient_AD >> 6) + 120;
			if (DispBrightness > 996) DispBrightness = 996;
			TIM_SetCompare1( TIM17, DispBrightness);
		}
		else
		{
			if (DispBrightness > 1000) DispBrightness = 1000;
			TIM_SetCompare1( TIM17, DispBrightness);
		}

		if (AutoManu & AUTOLEDS)
		{
			for (i=0; i<12; i++)
			{
				if  ( (RgbLedState.ComponentState[i] == LED_COMP_ON)
				   || (RgbLedState.ComponentState[i] == LED_COMP_BLINK_ON) )
				{
					RgbLedState.ActBrightComp[i] = ( RgbLedState.BrightComp[i] * (120 + (AnalogVals.Ambient_AD >> 6) ) ) >> 9;
					if (RgbLedState.ActBrightComp[i] > 1000) RgbLedState.ActBrightComp[i] = 1000;

					switch (i)
					{
						case 0:  TIM_SetCompare1( TIM1, RgbLedState.ActBrightComp[i]); break;
						case 1:  TIM_SetCompare2( TIM1, RgbLedState.ActBrightComp[i]); break;
						case 2:  TIM_SetCompare3( TIM1, RgbLedState.ActBrightComp[i]); break;
						case 3:  TIM_SetCompare4( TIM1, RgbLedState.ActBrightComp[i]); break;
						case 4:  TIM_SetCompare1( TIM4, RgbLedState.ActBrightComp[i]); break;
						case 5:	 TIM_SetCompare2( TIM4, RgbLedState.ActBrightComp[i]); break;
						case 6:  TIM_SetCompare3( TIM4, RgbLedState.ActBrightComp[i]); break;
						case 7:  TIM_SetCompare4( TIM4, RgbLedState.ActBrightComp[i]); break;
						case 8:  TIM_SetCompare1( TIM3, RgbLedState.ActBrightComp[i]); break;
						case 9:  TIM_SetCompare2( TIM3, RgbLedState.ActBrightComp[i]); break;
						case 10: TIM_SetCompare3( TIM3, RgbLedState.ActBrightComp[i]); break;
						case 11: TIM_SetCompare4( TIM3, RgbLedState.ActBrightComp[i]); break;
					}
				}
			}
		}
		else
		{
			if (LedBrightness > 1000) LedBrightness = 1000;
			for (i=0; i<12; i++)
				RgbLedState.ActBrightComp[i] = ( RgbLedState.BrightComp[i] * LedBrightness ) >> 10;
		}
	}
	else
	{
		if (DispBrightness > 1000) DispBrightness = 1000;
		if (LedBrightness > 1000) LedBrightness = 1000;
		TIM_SetCompare1( TIM17, DispBrightness);
		for (i=0; i<12; i++)
			RgbLedState.ActBrightComp[i] = ( RgbLedState.BrightComp[i] * LedBrightness ) >> 10;
	}
}


/******************************************************************************************************
                                                SetLedsErr
-------------------------------------------------------------------------------------------------------
Schaltet die angegebenen ohne Timer (nur fuer Fehlerausgabe)

Parameter:
  Leds:     LEDs die ein/ausgeschaltet werden sollen. siehe defines LED_xxx
  State:    ON oder OFF
******************************************************************************************************/
void SetLedsErr( u32 Leds, OnOffState State ) 
{
/*
	if ( State == OFF )
	{
		if ( Leds & LED_LRED ) LED_LRED_LOW;
		if ( Leds & LED_PRED ) GPIO_ResetBits( PORT_POWERLEDS, LED_PRED_PIN );
	}
	else
	{
		if ( Leds & LED_LRED ) LED_LRED_HIGH;
		if ( Leds & LED_PRED ) GPIO_SetBits( PORT_POWERLEDS, LED_PRED_PIN );
	}
*/
}

/******************************************************************************************************
                                              strreplace
-------------------------------------------------------------------------------------------------------
Ersetzt in einem String alle Vorkommen eines Zeichens OldChar durch das Zeichen NewChar

Parameter:
   String:   String in dem ersetzt werden soll
   OldChar:  Altes Zeichen das ersetzt werden soll
   NewChar:  Neues Zeichen das anstelle des alten Zeichens eingesetzt werden soll

Return: Zeiger auf String
******************************************************************************************************/
char *strreplace( char *String, char OldChar, char NewChar )
{
	while ( *String != 0 )
	{
		if ( *String == OldChar ) *String = NewChar;
		String++;
	}
	return String;
}

/******************************************************************************************************
                                               atan_tbl
-------------------------------------------------------------------------------------------------------
Ermittelt den Arcustangens von y/x mit Hilfe einer Tabelle. Die Genauigkeit liegt bei +-0.05�
Optimiert auf hohe Geschwindigkeit (STM32 mit 72MHz: 3-3.6�s).
Um die m�gliche Genauigkeit von +-0.05� ausnutzen zu k�nnen m�ssen die x und y Eingangswerte mindestens
den Bereich +-8192 verwenden.

Parameter:
   x:    X-Wert der kartesischen Position.
   y:    Y-Wert der kartesischen Position.

Return: Winkel in 1/10� Grad. z.B. 112 => 11.2�. Ausgegebener Wertebereich ist 0 ... 3599.
******************************************************************************************************/
u16 atan_tbl( s16 x, s16 y )
{
	u8  quad;
	u8  region;
	u32 tl;
	u16 ts;
	u16 idx;
	u16 idxdiff;
	const u16 *tblptr;
	const u16 *maxtblptr = &tantable[TANTBLSIZE];

	// Erstmal die Sonderf�lle abfangen und den Quadranten ermitteln
	if ( x == 0 )
	{
		if ( y == 0 ) { return 0; }
		else if ( y > 0 ) { return 900; }
		else { return 2700; }
	}
	else if ( x > 0 )
	{
		if ( y == 0 ) { return 0; }
		else if ( y > 0 ) { quad = 1; }
		else { quad = 4; y = -y; }
	}
	else
	{
		if ( y == 0 ) { return 1800; }
		else if ( y > 0 ) { quad = 2; }
		else { quad = 3; y = -y; }
		x = -x;
	}

	// x und y sind jetzt immer positiv, also im ersten Quadranten (0..90�). Jetzt wird bestimmt ob
	// wir unter oder �ber 45� liegen. Es wird noch ausgenutzt dass atan(y/x) = 90� - atan(x/y) ist.
	// Dadurch kann der zu verarbeitende Wert auf 0..1 (0..45�) begrenzt werden.
	// Hier wird jetzt auch x/y * 2^16 bzw. y/x * 2^16 gerechnet
	if ( x > y )
	{
		region = 0; tl = y;
		tl <<= 16; tl /= x;
	}
	else
	{
		region = 1; tl = x;
		tl <<= 16; tl /= y;
	}

	if ( tl >= 65536 ) 		// tl == 65536 wenn x=y war, also 45�
	{ idx = 450; }
	else
	{
		// Jetzt den Tabelleneintrag finden der dem ts Wert entspricht. Verfahren wie Sukzessive
		// Approximation, also in der tabellenmitte beginnen und je nachdem ob der Tabellenwert
		// gr�sser oder kleiner als ts ist um die halbe Schrittweite nach vorne oder hinten springen
		ts = (u16)tl;
		idxdiff = TANTBLSIZE / 4;
		tblptr = tantable + (TANTBLSIZE / 2);     // in der Mitte der Tabelle beginnen un
		do
		{
			if ( ts > *tblptr ) { tblptr += idxdiff; }
			else { tblptr -= idxdiff; }
			idxdiff /= 2;
		} while ( idxdiff > 0 );

		// Da oben nicht mit einer Tabellengr�sse gearbeitet wurde die einer Zweierpotenz entspricht
		// kann der gefundene Tabelleneintrag noch max 4 Schritte vom richtigen Eintrag entfernt sein
		if ( ts > *tblptr )
		{
			while ( tblptr < maxtblptr )
			{
				if ( ts < *tblptr ) break;
				tblptr++;
			}
		}
		else
		{
			while ( tblptr > tantable )
			{
				if ( ts >= *(tblptr-1) ) break;
				tblptr--;
			}
		}
		idx = tblptr - tantable;
	}

	// jetzt noch die Region und den Quadranten ber�cksichtigen
	if ( region == 1 ) idx = 900 - idx;
	if ( quad == 2 ) { idx = 1800 - idx; }
	else if ( quad == 3 ) { idx += 1800; }
	else if ( quad == 4 ) { idx = 3600 - idx; }
	return idx;
}
