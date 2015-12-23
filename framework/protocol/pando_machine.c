/*******************************************************
 * File name: pando_machine.c
 * Author:    Zhao Wenwu
 * Versions:  0.1
 * Description: APIs related to different machine platform, 
     Such as basic IO operation and some common functions.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 #include <time.h>

#include "pando_machine.h"
#include "../platform/include/pando_types.h"

#ifdef ESP8266_PLANTFORM

uint64_t FUNCTION_ATTRIBUTE pd_get_timestamp()
{
	uint64_t time_now;
	//time_now = pando_get_system_time();
	return time_now;
}

#else
uint64_t FUNCTION_ATTRIBUTE pd_get_timestamp()
{
/*
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec * 1000 + now.tv_usec/1000);
*/
    return 0;
}

#endif
