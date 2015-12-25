/******************************************************* 
* File name锛� timer4.h
* Author   :  pd
* Versions :  1.0
* Description: timer4 set
* History: 
*   1.Date:    2015-7-2
*     Author:  pd
*     Action:  create
*********************************************************/ 

#ifndef _TIMER4_H_
#define _TIMER4_H_

#include <stdint.h>

typedef void (*time4_callback)(void *arg);

/******************************************************************************
 * FunctionName : timer4_init
 * Description  : init timer4.
 * Parameters   : time_set: the set time, uint is ms.
 * Returns      :
 ******************************************************************************/
void timer4_init(uint16_t time_set, uint8_t repeat_flag, time4_callback time4_cb);

/******************************************************************************
 * FunctionName : timer1_start
 * Description  : start timer1.
 * Parameters   :
 * Returns      :
 ******************************************************************************/
void timer4_start(void);

/******************************************************************************
 * FunctionName : timer1_stop
 * Description  : stop timer1.
 * Parameters   :
 * Returns      :
 ******************************************************************************/
void timer4_stop(void);

#endif /* _PANDO_TIMER_H_ */

