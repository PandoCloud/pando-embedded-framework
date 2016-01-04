/*******************************************************
 * File name: peri_motor.h
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: motor.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef __PERI_MOTOR_H__
#define __PERI_MOTOR_H__

#include "c_types.h"
#include "driver/pwm.h"
#include "peri_rgb_light.h"

#pragma pack(1)


void peri_motor_init(struct PWM_APP_PARAM motor_param,struct PWM_INIT motor_init);
struct PWM_APP_PARAM peri_motor_get(void);
void peri_motor_set( struct PWM_APP_PARAM motor_param);


#endif
