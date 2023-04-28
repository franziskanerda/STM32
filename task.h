#ifndef __TASK_H
#define __TASK_H 1
/**********   #INCLUDES            **********/

/**********   #DEFINES             **********/
// Tasks von Index 0 bis Index MAX_TASKNR vorhanden.
#define MAX_TASKNR 		1
#define ILLEGAL_TASKNR  0xFFFF

#define RX_TASK            0
#define SAGE_TASK          1

/**********   ENUMS                **********/
/**********   STRUCTS              **********/
/**********   CONSTS               **********/
/**********   Public Variables     **********/

/**********   Public Functions     **********/
void InitTaskList( void );
void TaskList( void );
void TaskLoopEndless( void );
void SetTaskState( u16 TaskNr, void (*NewState)( void ) );
void StopTask( u16 TaskNr );
u16  SuspendTask( void );
void ResumeTask( u16 TaskNr );
void( *GetTaskState( u16 TaskNr ) ) ( void );
u32  IsTaskStopped( u16 TaskNr );
u32  IsTaskSuspended( u16 TaskNr );
void RxTask( void );

#endif // ifndef __TASK_H
