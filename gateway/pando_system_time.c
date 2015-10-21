/*******************************************************
 * File name: pando_system_time.h
 * Author:  Chongguang Li
 * Versions: 1.0
 * Description:This module is the interface of system time.
 * History:
 *   1.Date:  
 *     Author:
 *     Modification:    
 *********************************************************/

#include "pando_system_time.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"

static uint64 system_time = 0;

static os_timer_t  system_timer;
 
/******************************************************************************
 * FunctionName : pando_set_system_time
 * Description  : configure the system time.
 * Parameters   : the set value.
 * Returns      : none
*******************************************************************************/

void ICACHE_FLASH_ATTR
pando_set_system_time(uint64 time)
{
    system_time = time;

    int i = 0;
    for(i = 0; i<sizeof(system_time); i++)
    {
    	PRINTF("%02x ", *((unsigned char*)(&system_time) +i));
    }

    PRINTF("\n");
}

/******************************************************************************
 * FunctionName : pando_get_system_time
 * Description  : geit the system time.
 * Parameters   : none.
 * Returns      : the system time.
*******************************************************************************/

uint64 ICACHE_FLASH_ATTR
pando_get_system_time(void)
{
    return system_time;
}

/******************************************************************************
 * FunctionName : add_time.
 * Description  : add the system time at fixed time.
 * Parameters   : the time callback parameter.
 * Returns      : none.
*******************************************************************************/

static void ICACHE_FLASH_ATTR
add_time(void *arg)
{
    system_time = system_time + 60000;
    int i = 0;
    for(i = 0; i<sizeof(system_time); i++)
    {
    	PRINTF("%02x ", *((unsigned char*)(&system_time) +i));
    }

    PRINTF("\n");

#ifdef DEBUG_MSG
    system_print_meminfo();
    PRINTF("available heap size:%d\n", system_get_free_heap_size());
#endif

}

/******************************************************************************
 * FunctionName : pando_system_time_init
 * Description  : initialize system time.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/

void ICACHE_FLASH_ATTR
pando_system_time_init(void)
{
    os_timer_disarm(&system_timer);
    os_timer_setfn(&system_timer, (os_timer_func_t *)add_time, NULL);
    os_timer_arm(&system_timer, 60000, 1);	
     
}

