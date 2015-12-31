/*******************************************************
 * File name: peri_led.c
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is driver of the led.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#include "peri_led.h"
#include "eagle_soc.h"
#include "driver/tisan_gpio.h"
#include "os_type.h"
#include "gpio.h"
#include "osapi.h"

static os_timer_t  led_blink_timer;
static uint32 led_pin;

/******************************************************************************
 * FunctionName : peri_led_init.
 * Description  : initialize the led.
 * Parameters   : uint8 gpio_id: the gpio pin drive the led.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_led_init(uint8 gpio_id)
{
	PIN_FUNC_SELECT(tisan_get_gpio_name(gpio_id), tisan_get_gpio_general_func(gpio_id));
	PIN_PULLUP_EN(tisan_get_gpio_name(gpio_id));
	led_pin = gpio_id;
}

/******************************************************************************
 * FunctionName : led_blink_timer_cb.
 * Description  : the led_blink_time callback function..
 * Parameters   : none.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
led_blink_timer_cb(void* arg)
{
	static bool state = 0;
	if(state == 0)
	{
		state = 1;
		GPIO_OUTPUT_SET(led_pin, state);
	}
	else
	{
		state = 0;
		GPIO_OUTPUT_SET(led_pin, state);
	}
}

/******************************************************************************
 * FunctionName : peri_led_blink.
 * Description  : blink the led.
 * Parameters   : BLINK_SPEED speed: the blink speed.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_led_blink(BLINK_SPEED speed)
{
	os_timer_disarm(&led_blink_timer);
	os_timer_setfn(&led_blink_timer, (os_timer_func_t *)led_blink_timer_cb, NULL);
	os_timer_arm(&led_blink_timer, speed, 1);
}

/******************************************************************************
 * FunctionName : peri_led_set.
 * Description  : set the led state.
 * Parameters   : bool state: the set state of the led, 1:on ; 0:off.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_led_set(bool state)
{
	os_timer_disarm(&led_blink_timer);
	GPIO_OUTPUT_SET(led_pin, state);
}
