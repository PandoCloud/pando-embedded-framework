/*******************************************************
 * File name: pando_system_time.h
 * Author:  Chongguang Li
 * Versions: 1.0
 * Description:This module is the interface of system time, generate by time interrupt.
 * History:
 *   1.Date:  
 *     Author:
 *     Modification:    
 *********************************************************/

#ifndef PANDO_SYSTEM_TIME_H
#define PANDO_SYSTEM_TIME_H

#include "c_types.h"
 
/******************************************************************************
 * FunctionName : pando_set_system_time
 * Description  : configure the system time.
 * Parameters   : the set value.
 * Returns      : none
*******************************************************************************/

void pando_set_system_time(uint64 time);

/******************************************************************************
 * FunctionName : pando_get_system_time
 * Description  : geit the system time.
 * Parameters   : none.
 * Returns      : the system time.
*******************************************************************************/

uint64 pando_get_system_time(void);

/******************************************************************************
 * FunctionName : pando_system_time_init
 * Description  : initialize system time.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/


void pando_system_time_init(void);

#endif
