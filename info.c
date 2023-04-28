/**********   #INCLUDES            **********/
#include "system.h"
#include "info.h"
#include "serial.h"

extern void Reset_Handler( void );

/**********   Public Variables     **********/
__attribute__ ( ( section( ".app_start_str" ) ) )
const char app_start_str[] = "App-Start";

__attribute__ ( ( section( ".app_stop_str" ) ) )
const char app_stop_str[] = "App-Stop";

__attribute__ ( ( section( ".app_info" ) ) )

const appinfo_strct app_info =
{
   Reset_Handler, VERSION_NR, app_start_str, app_stop_str, __DATE__, __TIME__,
   RX_DATA_SIZE, TX_DATA_SIZE, CRC_PLACE_HOLDER
};
