/*******************************************************
 * File name: peri_jdq.h
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: relay.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef __PERI_JDQ_H__
#define __PERI_JDQ_H__

#include "c_types.h"

#pragma pack(1)

void peri_jdq_init(uint8 gpio_id);
uint8  peri_jdq_get(void);
void peri_jdq_set(uint8 on_off);
void peri_jdq_timer_set(void);
#endif
