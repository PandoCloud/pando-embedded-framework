/*********************************************************
 * File name: pando_timer.h
 * Author: xiaoxiao
 * Versions: 1.0
 * Description: declare the types.
 * History:
 *   1.Date: Oct 28, 2015
 *     Author: xiaoxiao
 *     Modification:
 *********************************************************/

#ifndef _PANDO_TIMER_H_
#define _PANDO_TIMER_H_

#include "pando_types.h"

typedef void (*expiry_cb)(void* arg);

//define the timer structure
struct pd_timer
{
    //the timer interval (millisecond)
    uint16_t interval;
    //whether repeat
    uint8_t repeated;
    uint8_t timer_no;
    //the function pointer to callback if expiry
    expiry_cb timer_cb;
    void* arg;
};

/******************************************************************************
 * FunctionName : timer1_init
 * Description  : init timer1.
 * Parameters   : timer_cfg: the config timer.
 * Returns      : 
 ******************************************************************************/
void pando_timer_init(struct pd_timer * timer);

/******************************************************************************
 * FunctionName : timer1_start
 * Description  : start timer1.
 * Parameters   : 
 * Returns      : 
 ******************************************************************************/
void pando_timer_start(struct pd_timer * timer);

/******************************************************************************
 * FunctionName : timer1_stop
 * Description  : stop timer1.
 * Parameters   : 
 * Returns      : 
 ******************************************************************************/
void pando_timer_stop(struct pd_timer * timer);

#endif /* _PANDO_TIMER_H_ */
