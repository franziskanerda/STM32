#ifndef __SYSLIB_H
#define __SYSLIB_H 1

/** #INCLUDES          *******************************************************************************/
#include "gpio_map.h"

/** #DEFINES           *******************************************************************************/
#define DbgOut( format, ... ) DbgPrintf( format, ## __VA_ARGS__ )

#define ERR_BLINK_CFG           		1
#define ERR_BLINK_MFG            		2
#define ERR_BLINK_EVENTERR       		5
#define ERR_BLINK_HEAPOVERFLOW   		7
#define ERR_BLINK_SYSEXIT        		30
#define ERR_BLINK_IRQNMI         		31
#define ERR_BLINK_IRQHARDFAULT   		32
#define ERR_BLINK_IRQMEMMANAGE   		33
#define ERR_BLINK_IRQBUSFAULT    		34
#define ERR_BLINK_IRQUSAGEFAULT  		35
#define ERR_BLINK_IRQSVC         		36
#define ERR_BLINK_IRQDEBUGMON    		37
#define ERR_BLINK_IRQPENDSV      		38
#define ERR_BLINK_IRQUNUSED      		39

#define SYSTICK_RELOAD_VALUE 			(10000 - 1)

// timer init - Zeitbasis in ms
#define TimeoutStart( start ) (start) = sys_tic

// @return true wenn timeout abgelaufen
#define Timeout( start, duration ) ( ( sys_tic - (start) ) >= (duration) )

#define NOAUTO							0
#define AUTODISP 						1
#define AUTOLEDS						2
#define AUTOLEDSDISP					3

#define IN_CHARGER_CURR_LIMIT_100MA    	0
#define IN_CHARGER_CURR_LIMIT_500MA   	1
#define IN_CHARGER_CURR_LIMIT_FULL    	2
#define IN_CHARGER_CURR_LIMIT_STANDBY 	3

#define LED_COMP_OFF					0
#define LED_COMP_CHANGED_TO_BLINK		1
#define LED_COMP_ON						2
#define LED_COMP_BLINK_ON				3
#define LED_COMP_BLINK_OFF				4

#define LED_RGB1_RED					0x0001
#define LED_RGB1_GREEN 					0x0002
#define LED_RGB1_YELLOW					0x0003
#define LED_RGB1_BLUE					0x0004
#define LED_RGB1_MAGENTA				0x0005
#define LED_RGB1_CYAN					0x0006
#define LED_RGB1_WHITE					0x0007

#define LED_RGB2_RED					0x0010
#define LED_RGB2_GREEN 					0x0020
#define LED_RGB2_YELLOW					0x0030
#define LED_RGB2_BLUE					0x0040
#define LED_RGB2_MAGENTA				0x0050
#define LED_RGB2_CYAN					0x0060
#define LED_RGB2_WHITE					0x0070

#define LED_RGB3_RED					0x0100
#define LED_RGB3_GREEN 					0x0200
#define LED_RGB3_YELLOW					0x0300
#define LED_RGB3_BLUE					0x0400
#define LED_RGB3_MAGENTA				0x0500
#define LED_RGB3_CYAN					0x0600
#define LED_RGB3_WHITE					0x0700

#define LED_RGB4_RED					0x1000
#define LED_RGB4_GREEN 					0x2000
#define LED_RGB4_YELLOW					0x3000
#define LED_RGB4_BLUE					0x4000
#define LED_RGB4_MAGENTA				0x5000
#define LED_RGB4_CYAN					0x6000
#define LED_RGB4_WHITE					0x7000

#define LED_WHITELED1_OFF				0x0000
#define LED_WHITELED1_TORCH				0x0001
#define LED_WHITELED1_STROBE			0x0002
#define LED_WHITELED1_FLASH				0x0004
#define LED_WHITELED2_OFF				0x0000
#define LED_WHITELED2_TORCH				0x0100
#define LED_WHITELED2_STROBE			0x0200
#define LED_WHITELED2_FLASH				0x0400

/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
#pragma pack(2)
typedef struct RgbLedState_s
{
	u16 GeneralBrightness;	// generell eingestellte Helligkeit der LEDs
	u32 FinalOffTime[12];	// endgueltige Abschaltzeit der LED-Komponenten
	u32 PeriodicOnTime[12];	// periodische Einschaltzeit der LED-Komponenten
	u32 PeriodicOffTime[12];// periodische Abschaltzeit der LED-Komponenten
	u16 ActBrightComp[12];	// Helligkeit der LED-Komponenten mit Umgebungslichtkompensation
	u16 BrightComp[12];		// Helligkeit der LED-Komponenten
	u32 FirstTime[12];		// Startzeit (benoetigt fuer FinalOff)
	u32 LastTime[12];		// zuletzt verwendete Zeit
	u8  ComponentState[12]; // tatsächlich eingestellter Status der LED-Komponente
} RgbLedStateStrct;

typedef struct FlashLedState_s
{
	u16 BrightComp[2];		// Helligkeit der LEDs
	u16 ModeMask;
	u16 PeriodicTime;		// Periodenzeit der LEDs (nur verwendet im Stroboskob-Modus)
	u16 OnTime;				// (periodische) Einschaltzeit der LEDs
} FlashLedStateStrct;
#pragma pack()

typedef struct DispBacklight_s
{
	u16 Mode;
	u16 Brightness;
} DispBacklightStruct;

typedef struct HwModuleMask_s
{
	u16 Available;
	u16 Error;
	u16 Usable;
	u16 OnOff;
} HwModuleMaskStruct;

#if PHW == 100

typedef struct AnalogVals_s
{
	u16 Usb_mV;
	u16 In1_mV;
	u16 In2_mV;
	u16 Ambient_AD;
	u16 Akku_mV;
	s16 Akku_mA;
	u16 InCurr_mA;
	u16 HW_Version;
	u16 Akku_Version;
	s16 Temp_STM;
	s16 Temp_Akku1;
	s16 Temp_Akku2;
} AnalogValStrct;

#else

typedef struct AnalogVals_s
{
	u16 Usb_mV;
	u16 In1_mV;
	u16 In2_mV;
	u16 Ambient_AD;
	u16 Akku_mV;
	u16 Akku_mV_LT;
	s16 Akku_mA;
	s16 Akku_mA_LT;
	u16 InCurr_mA;
	u16 HW_Version;
	u16 Akku_Version;
	u16 TempAkku1Raw;
	s16 TempAkku1Deg;
	u16 TempAkku2Raw;
	s16 TempAkku2Deg;
	s16 TempKuehlDeg;
} AnalogValStrct;

#endif

/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/
extern RgbLedStateStrct RgbLedState;
extern FlashLedStateStrct FlashLedState;
extern AnalogValStrct AnalogVals;
extern HwModuleMaskStruct HwModuleMask;
extern DispBacklightStruct DispBacklight;
extern u32 BeepOffTime;
extern u32 WLed1OffTime;
extern u32 WLed2OffTime;
extern u16 AmbVolt[], AkkVolt[], AkkVoltLt[], KuehlTemp[], UsbVolt[], In1Volt[], In2Volt[], InpCurr[];
extern s16 AkkCurr[], AkkCurrLt[], AkkTemp[];
extern GPIOMode_TypeDef PinMode[];

/** Public Functions   *******************************************************************************/
u8 *  wtoh( u16 Value, u8 *String );
u8 *  dwtoh( u32 Value, u8 *String );
u32   htodw( const u8 *String, u8 Len );
char *strreplace( char *String, char OldChar, char NewChar );
void  DelayMs( u16 Delay );
void  DelayUsRaw( u32 Delay );
u64   GetSystemUs( void );

void GpioInit( void );
void RCC_Configuration_Init( void );
void RCC_Configuration_Off( void );
void RCC_Configuration_On( void );
void RTC_Configuration( void );
void PWM_Configuration ( void );
void EXTI_Configuration( void );
void NVIC_Configuration( void );
void NVIC_DeInit( void );
void NVIC_SCBDeInit( void );
void ADC_Configuration ( void );
void DAC_Configuration ( void );
void I2C_Configuration ( void );
void RTC_SetTime( u32 Seconds );
void RTC_GetTime( u32 *Seconds );
void InitLedState( void );
s8   SetLedState( u16 ChangeMask, u16 BrightRed, u16 BrightGreen, u16 BrightBlue,
			    u32 FinalOffTime, u32 PeriodicOnTime, u32 PeriodicOffTime );
void CheckLedState( void );
void SetLedsErr( u32 Leds, OnOffState State );
void ErrorBlink( u16 errcode, bool RunTasks );
void SetFlashLedMode ( void );
void Beep( u16 Frequency, u16 Time );
void SetDispLedBrightness( u8 AutoManu, u16 DispBrightness, u16 LedBrightness );
void COut( char *format, ... );
void DbgPrintf( char *format, ... );
void DbgHex( void *data, u16 len );
void DbgPulse( u16 PulsCnt );
void SetChargerInputCurrentLimit( u8 Limit );
u16  atan_tbl( s16 x, s16 y );

#endif // SYSLIB_H
