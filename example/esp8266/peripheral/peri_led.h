/*******************************************************
 * File name: peri_led.h
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is driver of the led.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#ifndef APP_PERIPHERAL_PERI_LED_H_
#define APP_PERIPHERAL_PERI_LED_H_

#include "c_types.h"

typedef enum {
   BLINK_QUICK = 50,
   BLINK_SLOW = 1000
}BLINK_SPEED;

/******************************************************************************
 * FunctionName : peri_led_init.
 * Description  : initialize the led.
 * Parameters   : uint8 gpio_id: the gpio pin drive the led.
 * Returns      : none
*******************************************************************************/
void peri_led_init(uint8 gpio_id);

/******************************************************************************
 * FunctionName : peri_led_blink.
 * Description  : blink the led.
 * Parameters   : BLINK_SPEED speed: the blink speed.
 * Returns      : none
*******************************************************************************/
void peri_led_blink(BLINK_SPEED speed);

/******************************************************************************
 * FunctionName : peri_led_set.
 * Description  : set the led state.
 * Parameters   : bool state: the set state of the led, 1:on ; 0:off.
 * Returns      : none
*******************************************************************************/
void peri_led_set(bool state);

#endif /* APP_PERIPHERAL_PERI_LED_H_ */
