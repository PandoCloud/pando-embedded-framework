/*******************************************************
 * File name: user_tri_color_light.c
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: tri-colored light.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/

#include "peri_rgb_light.h"

#include "user_interface.h"
#include "driver/pwm.h"
#include "c_types.h"
#include "spi_flash.h"
#include "driver/tisan_gpio.h"


#define RGB_LIGHT_PRIV_SAVE 1

struct PWM_APP_PARAM light_r_param={25000,255,0,255};
struct PWM_APP_PARAM light_g_param={25000,0,255,255};
struct PWM_APP_PARAM light_b_param={25000,255,255,0};

/******************************************************************************
 * FunctionName : peri_rgb_light_param_get.
 * Description  : get the parameter of the RGB light.
 * Parameters   : none 
 * Returns      : the parameter of the RGB light.
*******************************************************************************/

struct PWM_APP_PARAM ICACHE_FLASH_ATTR
peri_rgb_light_param_get(void)
{
    struct PWM_APP_PARAM ret;
    
    spi_flash_read((PRIV_PARAM_START_SEC + RGB_LIGHT_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
    	(uint32 *)&ret, sizeof(struct PWM_APP_PARAM));
    return ret;
}

 
/******************************************************************************
 * FunctionName : peri_rgb_light_param_set.
 * Description  : set the parameter of the RGB light.
 * Parameters   : light_param-- RGB light parameter.
 * Returns      : none
*******************************************************************************/

void ICACHE_FLASH_ATTR 
peri_rgb_light_param_set( struct PWM_APP_PARAM light_param)
{
    pwm_set_freq(light_param.pwm_freq);
    pwm_set_duty(light_param.pwm_duty[0], 0); // red colour.
    pwm_set_duty(light_param.pwm_duty[1], 1); // green colour.
    pwm_set_duty(light_param.pwm_duty[2], 2); // blue colour.
    
    pwm_start();
    
    spi_flash_erase_sector(PRIV_PARAM_START_SEC + RGB_LIGHT_PRIV_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + RGB_LIGHT_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
	    (uint32 *)&light_param, sizeof(struct PWM_APP_PARAM));
}

/******************************************************************************
 * FunctionName : peri_rgb_light_param_timer_set.
 * Description  : set the parameter of the RGB light by timer.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/

void ICACHE_FLASH_ATTR
peri_rgb_light_param_timer_set(void)
{
	static uint8 j=0;

	if(j==0)
	{
		peri_rgb_light_param_set(light_r_param);
		PRINTF("red\n");
		j++;
	}
	else if(j==1)
	{
		peri_rgb_light_param_set(light_g_param);
		PRINTF("green\n");
		j++;
	}
	else
	{

		peri_rgb_light_param_set(light_b_param);
		PRINTF("blue\n");
		j=0;
	}


}
/******************************************************************************
 * FunctionName : peri_rgb_light_init
 * Description  : light demo initialize, mainly initialize pwm mode
 * Parameters   : struct PWM_APP_PARAM light_param,struct PWM_INIT light_init
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_rgb_light_init(struct PWM_APP_PARAM light_param,struct PWM_INIT light_init)
{
    
    PRINTF("I am the tri-colored light\n");
    PRINTF("pwm_freq: %d, pwm_duty_red: %d, pwm_duty_green: %d, pwm_duty_blue: %d\n", light_param.pwm_freq,
        (light_param.pwm_duty)[0], (light_param.pwm_duty)[1], (light_param.pwm_duty)[2]);

    spi_flash_write((PRIV_PARAM_START_SEC + RGB_LIGHT_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
    	    (uint32 *)&light_param, sizeof(struct PWM_APP_PARAM));
            
    pwm_init(light_param,light_init);
    pwm_start();
    
}



