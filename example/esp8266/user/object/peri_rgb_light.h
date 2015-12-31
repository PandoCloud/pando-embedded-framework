/*******************************************************
 * File name: user_tri_color_light.h
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: tri-colored light.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef ___PERI_RGB_LIGHT_H__
#define ___PERI_RGB_LIGHT_H__

#include "c_types.h"
#include "driver/pwm.h"

#pragma pack(1)



//void peri_rgb_light_init(struct PWM_APP_PARAM light_param,struct PWM_INIT light_init);
struct PWM_APP_PARAM peri_rgb_light_param_get(void);
void peri_rgb_light_param_set( struct PWM_APP_PARAM light_param);
void peri_rgb_light_param_timer_set(void);

#endif
