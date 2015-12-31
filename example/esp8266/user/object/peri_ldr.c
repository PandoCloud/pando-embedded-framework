/*******************************************************
 * File name: peri_ldr.c
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: light dependent resistor.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/

#include "peri_ldr.h"

#include "user_interface.h"
#include "driver/adc.h"
#include "driver/tisan_gpio.h"
#include "c_types.h"
#include "gpio.h"
/******************************************************************************
 * FunctionName : peri_lsr_read
 * Description  : read lsr value
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
uint16 ICACHE_FLASH_ATTR
peri_ldr_read(void)
{
   return adc_read();
}
/******************************************************************************
 * FunctionName : peri_lsr_timer_read
 * Description  : peri_lsr_timer_read
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_ldr_timer_read(void)
{
	uint16 value;
	value = peri_lsr_read();
	PRINTF("lsr_value=%d\n",value);
}

