/*******************************************************
 * File name: peri_jdq.c
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: relay.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/

#include "peri_jdq.h"

#include "user_interface.h"
#include "driver/pwm.h"
#include "c_types.h"
#include "spi_flash.h"
#include "gpio.h"
#include "peri_rgb_light.h"
#include "../pando/framework/subdevice/pando_object.h"
#include "driver/tisan_gpio.h"

uint8_t FLASH_SEC_NUM = 0 ;

#define JDQ_FLASH_PRIV_SAVE     FLASH_SEC_NUM
uint8 jdq_pin;
/******************************************************************************
 * FunctionName : peri_jdq_get.
 * Description  : get jdq state.
 * Parameters   : none
 * Returns      : jdq state.
*******************************************************************************/

uint8 ICACHE_FLASH_ATTR
peri_jdq_get()
{
    uint8  jdq_state;
    
    spi_flash_read((PRIV_PARAM_START_SEC + JDQ_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
    	(uint32 *)&jdq_state, sizeof(uint8));
    return jdq_state;
}
/******************************************************************************
 * FunctionName : peri_jdq_set
 * Description  : set jdq on or off
 * Parameters   : uint8 on_off
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_jdq_set(uint8 on_off)
{
	if(on_off==1)
	{
		 GPIO_OUTPUT_SET(jdq_pin, 1);
	     PRINTF("jdq ON\n");
	}
	else
	{
		 GPIO_OUTPUT_SET(jdq_pin, 0);
		 PRINTF("jdq OFF\n");
	}
    spi_flash_erase_sector(PRIV_PARAM_START_SEC + JDQ_FLASH_PRIV_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + JDQ_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
	    (uint32 *)&on_off, sizeof(uint8));


}
/******************************************************************************
 * FunctionName : peri_jdq_timer_set
 * Description  : set jdq on or off by timer
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_jdq_timer_set(void)
{
	static uint8 i=1;
	if(i==1)
	{
		 GPIO_OUTPUT_SET(jdq_pin, i);
	     PRINTF("jdq ON\n");
	     i=0;
	}
	else
	{
		 GPIO_OUTPUT_SET(jdq_pin, i);
		 PRINTF("jdq OFF\n");
		 i=1;
	}
    spi_flash_erase_sector(PRIV_PARAM_START_SEC + JDQ_FLASH_PRIV_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + JDQ_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
	    (uint32 *)&i, sizeof(uint8));


}
/******************************************************************************
 * FunctionName : peri_jdq_init
 * Description  : JDQ initialize, mainly initialize pwm mode
 * Parameters   : uint8 gpio_id
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_jdq_init(uint8 gpio_id)
{
   // enum JDQ_STATE jdq_state;

    PRINTF("I am the jdq\n");

    //PRINTF("jdq_state: %d\n", jdq_state);
    PIN_FUNC_SELECT(tisan_get_gpio_name(gpio_id), tisan_get_gpio_general_func(gpio_id));
    PIN_PULLUP_EN(tisan_get_gpio_name(gpio_id));
    jdq_pin=gpio_id;

}




