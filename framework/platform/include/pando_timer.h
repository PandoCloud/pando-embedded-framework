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

//define the timer structure
struct pd_timer
{
    //the timer interval (millisecond)
    uint16_t interval;
    //whether repeat
    uint8_t repeated;
    //the function pointer to callback if expiry
    void (*expiry_cb)(void);
};

/******************************************************************************
 * FunctionName : timer1_init
 * Description  : init timer1.
 * Parameters   : timer_cfg: the pointer to timer config.
 * Returns      : 
 ******************************************************************************/
void timer1_init(struct pd_timer *timer_cfg);

/******************************************************************************
 * FunctionName : timer1_start
 * Description  : start timer1.
 * Parameters   : 
 * Returns      : 
 ******************************************************************************/
void timer1_start(void);

/******************************************************************************
 * FunctionName : timer1_stop
 * Description  : stop timer1.
 * Parameters   : 
 * Returns      : 
 ******************************************************************************/
void timer1_stop(void);

#endif /* _PANDO_TIMER_H_ */
