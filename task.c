/** Description        *******************************************************************************/

/** #INCLUDES          *******************************************************************************/
#include <string.h>
#include <limits.h>
#include "system.h"
#include "syslib.h"
#include "globals.h"
#include "task.h"
#include "sage.h"
#include "serial.h"
#include "irq.h"

/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/
/** Public Variables   *******************************************************************************/
/** Public Functions   *******************************************************************************/

/** Private Variables  *******************************************************************************/
// Array mit den Zeigern auf die Taskfunktionen
void( *volatile TaskFnct[MAX_TASKNR+1] ) ( void );

// Array mit den Zeigern auf die Backup-Taskfunktionen
void( *volatile TaskFnctBck[MAX_TASKNR+1] ) ( void );

// Nummer des momentan laufenden Tasks
volatile u16 ActTaskNr;

// Ein gesetztes Bit zeigt an das der Task aktiv ist (nicht auf TaskStopped steht)
// TaskFnct[0] => Bit 0,  TaskFnct[1] => Bit 1 usw
// Wenn die ganze Variable auf 0 steht kann der Prozessor schlafen gehen
volatile u32 ActiveTasks;

/** Private Functions  *******************************************************************************/
void TaskStopped( void );
void TaskSuspended( void );


/** Code               *******************************************************************************/

/******************************************************************************************************
                                             InitTaskList
-------------------------------------------------------------------------------------------------------
Initialisiere die Funktionspointer in der TaskListe.
******************************************************************************************************/
void InitTaskList( void ) 
{
   u16 n;

   for ( n = 0 ; n <= MAX_TASKNR ; n++ ) 
     TaskFnct[n] = TaskStopped;

   ActTaskNr = 0;
   ActiveTasks = 0;
}

/******************************************************************************************************
                                               TaskList
-------------------------------------------------------------------------------------------------------
Führt alle Tasks einmal aus.
******************************************************************************************************/
void TaskList( void ) 
{
   u32 n;

   // Jeden anderen Task einmal aufrufen bevor wir weitermachen.
   for ( n = 0 ; n <= MAX_TASKNR ; n++ ) 
   {
      ActTaskNr++;
      if ( MAX_TASKNR < ActTaskNr ) 
         ActTaskNr = 0;
      (*TaskFnct[ActTaskNr])();
   }
}

/******************************************************************************************************
                                            TaskLoopEndless
-------------------------------------------------------------------------------------------------------

******************************************************************************************************/
void TaskLoopEndless( void ) 
{
   u32 n;

   while ( 1 ) 
   {
      for ( n = 0 ; n <= MAX_TASKNR ; n++ ) 
      {
         ActTaskNr++;
         if ( MAX_TASKNR < ActTaskNr ) ActTaskNr = 0;
         (*TaskFnct[ActTaskNr])();
      }
      // Wenn alle Tasks angehalten sind wird der Prozessor gestoppt.
      // Er wacht beim nächsten IRQ auf. Beim beenden des IRQ stoppt der Prozessor von selbst
      // wieder ausser wenn dies explizit im IRQ ausgeschaltet wird.
      if ( ActiveTasks == 0 ) 
      {
         SCB->SCR |= NVIC_LP_SLEEPONEXIT;   // Nach IRQ wieder stoppen
         __WFI();                           // WaitForInterrupt = Prozessor stoppen
      }
   }
}

/******************************************************************************************************
                                             SetTaskState
-------------------------------------------------------------------------------------------------------
Setzt einen Task auf einen neuen State

Parameter:
   TaskNr:   siehe die in task.h definierten Task-Nummern
   NewState: die neue TaskFunktion
******************************************************************************************************/
void SetTaskState( u16 TaskNr, void (*NewState)( void ) ) 
{
   TaskFnct[TaskNr] = NewState;
   if ( NewState == TaskStopped ) 
      RST( ActiveTasks, TaskNr );
   else 
      SET( ActiveTasks, TaskNr );
}

/******************************************************************************************************
                                               StopTask
-------------------------------------------------------------------------------------------------------
Setzt den Task auf gestoppt

Parameter:
   TaskNr:   siehe die in task.h definierten Task-Nummern
******************************************************************************************************/
void StopTask( u16 TaskNr ) 
{
   TaskFnct[TaskNr] = TaskStopped;
   RST( ActiveTasks, TaskNr );
}

/******************************************************************************************************
                                              SuspendTask
-------------------------------------------------------------------------------------------------------
Der aufrufende Task wird schlafen gelegt indem sein Zeiger gebackuped wird und dann auf TaskSuspended
gesetzt wird.

Return: Nummer des Tasks der suspendet wurde. Wird für ResumeTask gebraucht
******************************************************************************************************/
u16 SuspendTask( void ) 
{
   if ( (TaskFnct[ActTaskNr] == TaskStopped) OR (TaskFnct[ActTaskNr] == TaskSuspended) ) 
      return ILLEGAL_TASKNR;
   TaskFnctBck[ActTaskNr] = TaskFnct[ActTaskNr];
   TaskFnct[ActTaskNr] = TaskSuspended;
   RST( ActiveTasks, ActTaskNr );
   return ActTaskNr;
}

/******************************************************************************************************
                                              ResumeTask
-------------------------------------------------------------------------------------------------------
Der aufrufende task wird wieder aufgeweckt indem sein Zeiger auf den Backupwert gesetzt wird.

Parameter:
   TaskNr:   Nummer des Tasks der aufgeweckt werden soll
******************************************************************************************************/
void ResumeTask( u16 TaskNr ) 
{
   if ( TaskNr != ILLEGAL_TASKNR ) 
   {
      TaskFnct[TaskNr] = TaskFnctBck[TaskNr];
      SET( ActiveTasks, ActTaskNr );
   }
}

/******************************************************************************************************
                                             GetTaskState
-------------------------------------------------------------------------------------------------------
Liefert den Funktionszeiger auf dem der angegebene Task steht zurück.

Parameter:
   TaskNr:   siehe die in task.h definierten Task-Nummern

Return: Zeiger auf Funktion auf dem der Task gerade steht
******************************************************************************************************/
void( *GetTaskState( u16 TaskNr ) ) ( void ) 
{
   return TaskFnct[TaskNr];
}

/******************************************************************************************************
                                             IsTaskStopped
-------------------------------------------------------------------------------------------------------
Prüft ob der Task mit der angegebenen Nummer gestoppt ist

Parameter:
   TaskNr:   siehe die in task.h definierten Task-Nummern

Return: 1 wenn der Task gestoppt ist
        0 wenn der Task nicht gestoppt ist
******************************************************************************************************/
u32 IsTaskStopped( u16 TaskNr ) 
{
   return TaskFnct[TaskNr] == TaskStopped ;
}

/******************************************************************************************************
                                             IsTaskSuspended
-------------------------------------------------------------------------------------------------------
Prüft ob der Task mit der angegebenen Nummer suspended ist

Parameter:
   TaskNr:   siehe die in task.h definierten Task-Nummern

Return: 1 wenn der Task suspended ist
        0 wenn der Task nicht suspended ist
******************************************************************************************************/
u32 IsTaskSuspended( u16 TaskNr ) 
{
   return TaskFnct[TaskNr] == TaskSuspended ;
}

/******************************************************************************************************
                                               TaskStopped
-------------------------------------------------------------------------------------------------------
Funktion auf die ein TaskFunktionsPointer zeigt, wenn er deaktiviert ist
******************************************************************************************************/
void TaskStopped( void ) 
{}

/******************************************************************************************************
                                               TaskSuspended
-------------------------------------------------------------------------------------------------------
Funktion auf die ein TaskFunktionsPointer zeigt, wenn er vorübergehend unterbrochen ist
******************************************************************************************************/
void TaskSuspended( void ) 
{}

/******************************************************************************************************
                                                RxTask
-------------------------------------------------------------------------------------------------------
Führt nur den Empfangstask aus für ErrorBlink
******************************************************************************************************/
void RxTask( void ) 
{
   (*TaskFnct[RX_TASK])();
}
