/*******************************************************
 * File name: pando_device_login.h
 * Author: ruizeng
 * Versions: 1.0
 * Description:This module is to used to login device via pando cloud api.
 * History:
 *   1.Date: initial code
 *     Author: ruizeng
 *     Modification:    
 *********************************************************/
#ifndef __PANDO_DEVICE_LOGIN_H
#define __PANDO_DEVICE_LOGIN_H

#include "gateway_defs.h"

 /******************************************************************************
 * FunctionName : pando_device_login
 * Description  : try login device using pando cloud device register api.
 * Parameters   : the specify login callback function.
 * Returns      : 
*******************************************************************************/
void pando_device_login(gateway_callback callback);

#endif
