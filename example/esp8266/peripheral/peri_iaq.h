/*******************************************************
 * File name: user_plug.h
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is driver of the subdevice: plug.
               in this example:GPIO 15: the plug relay driver;
                               GPIO 12  a blinking led;
                               GPIO 0   the wifi connect state indicate;
                               GPIO 13  a press key.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/

#ifndef __PERI_IAQ_H__
#define __PERI_IAQ_H__

#include "ets_sys.h"


void peri_iaq_init(void);
uint16 peri_iaq_read(void);
uint8* peri_iaq_get(void);


#endif

