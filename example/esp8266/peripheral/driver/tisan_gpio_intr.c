/*
 * tisan_gpio_intr.c
 *
 *  Created on: 2015Äê10ÔÂ29ÈÕ
 *      Author: Administrator
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h"
#include "../../pando/framework/subdevice/pando_subdevice.h"
#include "tisan_gpio_intr.h"


void gpio_intr_handler(void *arg)
{
    uint8 i;
    uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

    PRINTF("Get into gpio_intr_handler, gpio_status:%d\n", gpio_status);

	if (gpio_status & BIT(((struct key_param *)arg)->gpio_id))
	{
		struct key_param *key = (struct key_param *)arg;

		//disable interrupt
		gpio_pin_intr_state_set(GPIO_ID_PIN(key->gpio_id), GPIO_PIN_INTR_DISABLE);
		//clear interrupt status
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(key->gpio_id));

		if (key->key_level == 1)
		{
			// 5s, restart & enter softap mode
			os_timer_disarm(&key->key_5s);
			os_timer_setfn(&key->key_5s, (os_timer_func_t *)key_5s_cb, key);
			os_timer_arm(&key->key_5s, LONG_PRESS_COUNT, 0);
			key->key_level = 0;
			gpio_pin_intr_state_set(GPIO_ID_PIN(key->gpio_id), GPIO_PIN_INTR_POSEDGE);
		}
		else
		{
			// 50ms, check if this is a real key up
			os_timer_disarm(&key->key_50ms);
			os_timer_setfn(&key->key_50ms, (os_timer_func_t *)key_50ms_cb, key);
			os_timer_arm(&key->key_50ms, 50, 0);
		}
	}

}


