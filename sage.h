#ifndef __SAGE_H
#define __SAGE_H 1

/** #INCLUDES          *******************************************************************************/
/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/

/** Public Functions   *******************************************************************************/
void SageTsk_Init( void );
void SageTsk_CheckForAction( void );
void CableOrAccuChanged_Handler( void );
void EnableCharging( bool On );
void DoSysAdMeasurements( void );
void LedTest(void);
#endif // SAGE_H