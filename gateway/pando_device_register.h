/*******************************************************
 * File name: pando_device_register.h
 * Author: ruizeng
 * Versions: 1.0
 * Description:This module is to used to register  device via pando cloud api.
 * History:
 *   1.Date: initial code
 *     Author: ruizeng
 *     Modification:    
 *********************************************************/
#ifndef __PANDO_DEVICE_REGISTER_H
#define __PANDO_DEVICE_REGISTER_H

#include "gateway_defs.h"

 /******************************************************************************
 * FunctionName : pando_device_register
 * Description  : register pando device using pando cloud device register api.
 * Parameters   : the specify callback function.
 * Returns      : 
*******************************************************************************/
void pando_device_register(gateway_callback callback);

#endif
