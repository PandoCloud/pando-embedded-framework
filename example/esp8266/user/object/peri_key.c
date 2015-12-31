/*******************************************************
 * File name: peri_key.c
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This is the key function module.

 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/
#include "peri_key.h"
#include "user_interface.h"
#include "eagle_soc.h"
#include "driver/key.h"
#include "os_type.h"
#include "mem.h"
#include "driver/tisan_gpio_intr.h"
#include "driver/tisan_gpio.h"
 

/******************************************************************************
 * FunctionName : user_plug_short_press
 * Description  : key's short press function, needed to be installed
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_key_short_press(void)
{
	  PRINTF("short\n");
}


/******************************************************************************
 * FunctionName : user_plug_long_press
 * Description  : key's long press function, needed to be installed, preserved function.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

void ICACHE_FLASH_ATTR
peri_key_long_press(void)
{
	  PRINTF("long\n");
}
/******************************************************************************
 * FunctionName : peri_key_init.
 * Description  : initialize key device.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_single_key_init(uint8 gpio_id,key_function long_press, key_function short_press)
{
    struct key_param *single_key = (struct key_param *)os_zalloc(sizeof(struct key_param));
    uint32 gpio_name=tisan_get_gpio_name(gpio_id);
    uint8 gpio_func=tisan_get_gpio_general_func(gpio_id);
    single_key->gpio_id = gpio_id;
    single_key->key_level = 1;
    single_key->long_press = long_press;
    single_key->short_press = short_press;

    ETS_GPIO_INTR_ATTACH(gpio_intr_handler, single_key);
    ETS_GPIO_INTR_DISABLE();

    key_init(gpio_name, gpio_id, gpio_func);

    ETS_GPIO_INTR_ENABLE();

}
