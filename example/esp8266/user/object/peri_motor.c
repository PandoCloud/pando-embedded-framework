/*******************************************************
 * File name: peri_motor.c
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: motor.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/

#include "peri_motor.h"

#include "../pando/framework/subdevice/pando_object.h"
#include "peri_rgb_light.h"

#include "user_interface.h"
#include "driver/pwm.h"
#include "c_types.h"
#include "spi_flash.h"
#include "driver/tisan_gpio.h"

extern uint8_t FLASH_SEC_NUM ;


#define MOTOR_FLASH_PRIV_SAVE     FLASH_SEC_NUM

struct PWM_APP_PARAM motor_forward_param={25000,40,0};
struct PWM_APP_PARAM motor_backard_param={25000,0,40};

/******************************************************************************
 * FunctionName : peri_motor_get.
 * Description  : get the parameter of the motor status.
 * Parameters   : none 
 * Returns      : the parameter of the motor status.
*******************************************************************************/

struct PWM_APP_PARAM ICACHE_FLASH_ATTR
peri_motor_get(void)
{
    struct PWM_APP_PARAM ret;
    
    spi_flash_read((PRIV_PARAM_START_SEC + MOTOR_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
    	(uint32 *)&ret, sizeof(struct PWM_APP_PARAM));
    return ret;
}

 
/******************************************************************************
 * FunctionName : peri_motor_set.
 * Description  : set the parameter of motor status.
 * Parameters   : struct LIGHT_PARAM motor_param.
 * Returns      : none
*******************************************************************************/

void ICACHE_FLASH_ATTR 
peri_motor_set( struct PWM_APP_PARAM motor_param)
{
    pwm_set_freq(motor_param.pwm_freq);
    pwm_set_duty(motor_param.pwm_duty[0], 0);
    pwm_set_duty(motor_param.pwm_duty[1], 1);
    pwm_set_duty(motor_param.pwm_duty[2], 2);
    
    pwm_start();
    
    spi_flash_erase_sector(PRIV_PARAM_START_SEC + MOTOR_FLASH_PRIV_SAVE);
	spi_flash_write((PRIV_PARAM_START_SEC + MOTOR_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
	    (uint32 *)&motor_param, sizeof(struct PWM_APP_PARAM));
}

/******************************************************************************
 * FunctionName : peri_motor_init
 * Description  : motor initialize , mainly initialize pwm mode
 * Parameters   : struct LIGHT_PARAM motor_param,struct LIGHT_INIT motor_init
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_motor_init(struct PWM_APP_PARAM motor_param,struct PWM_INIT motor_init)
{
    PRINTF("I am the electronic-motor\n");
    PRINTF("pwm_freq: %d, pwm_duty1: %d, pwm_duty2: %d\n",motor_param.pwm_freq,
        (motor_param.pwm_duty)[0], (motor_param.pwm_duty)[1]);
    PRINTF("start flash write \n");

    spi_flash_write((PRIV_PARAM_START_SEC + MOTOR_FLASH_PRIV_SAVE) * SPI_FLASH_SEC_SIZE,
    	    (uint32 *)&motor_param, sizeof(struct PWM_APP_PARAM));

    PRINTF("finished \n");
    pwm_init(motor_param,motor_init);
    pwm_start();
    
}



