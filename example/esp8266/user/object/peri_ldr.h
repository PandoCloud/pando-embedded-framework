/*******************************************************
 * File name: peri_ldr.h
 * Author:
 * Versions:1.0
 * Description:This module is driver of the subdevice: light dependent resistor.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef __PERI_LDR_H__
#define __PERI_LDR_H__

#include "c_types.h"

#pragma pack(1)

uint16 peri_ldr_read(void);
void peri_ldr_timer_read(void);

#endif
