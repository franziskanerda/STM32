/** Description        *******************************************************************************/
// Global benutzte Variablen

/** #INCLUDES          *******************************************************************************/
#include "system.h"
#include "globals.h"

/** #DEFINES           *******************************************************************************/
/** ENUMS              *******************************************************************************/
/** STRUCTS            *******************************************************************************/
/** CONSTS             *******************************************************************************/


/** Public Variables   *******************************************************************************/
volatile u32 sys_tic = 0;
u32 sys_status = 0;
u8  PowerState = 0;
u32 LastCommTime = 0;
u16 GlobalEvent = 0;
HwInfoDataStrct HwInfoData;
ModMaskStrct  ModMaskData;

extern const BatDataStrct _BatDataSectionStartAdr;     		// aus Linkerscript
const BatDataStrct *const BatData = &_BatDataSectionStartAdr;
extern const MfgDataStrct _MfgDataSectionStartAdr;     		// aus Linkerscript
const MfgDataStrct *const MfgData = &_MfgDataSectionStartAdr;

/** Public Functions   *******************************************************************************/
/** Private Variables  *******************************************************************************/
/** Private Functions  *******************************************************************************/
