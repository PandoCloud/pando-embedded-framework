/*******************************************************
 * File name: peri_key.h
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is the key function module.

 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#ifndef PERI_KEY_H_
#define PERI_KEY_H_


#include "gpio.h"
#include "../../include/os_type.h"

#define CONFIG_KEY_0_IO_MUX     PERIPHS_IO_MUX_GPIO4_U

typedef void (* key_function)(void);

struct key_param {
    uint8 key_level;
    uint8  gpio_id;
    os_timer_t key_5s;
    os_timer_t key_50ms;
    key_function short_press;
    key_function long_press;
};

struct keys_param {
    uint8 key_num;
    struct key_param **single_key;
};

void peri_key_short_press(void);
void peri_key_long_press(void);
void peri_single_key_init(uint8 gpio_id,key_function long_press, key_function short_press);


#endif /* APP_INCLUDE_USER_USER_KEY_H_ */
