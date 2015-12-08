/*******************************************************
 * File name: gateway_defs.h
 * Author:Chongguang Li
 * Versions:1.0
 * Description:This module is to used to define the parameter and structure used in gateway.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef __GATEWAY_DEFS_H__
#define __GATEWAY_DEFS_H__

#include "../platform/include/pando_types.h"

#define PANDO_API_URL "http://api.pandocloud.com"
#define DATANAME_DEVICE_ID "pando_device_id"
#define DATANAME_DEVICE_SECRET "pando_device_secret"
#define DATANAME_DEVICE_KEY "pando_device_key"
#define DATANAME_ACCESS_TOKEN "pando_access_token"
#define DATANAME_ACCESS_ADDR "pando_access_addr"

#define PANDO_DEVICE_MODULE "SIM800_GPRS"

#define PANDO_SDK_VERSION "0.0.1"

typedef void(*gateway_callback)(sint8_t result);

#define PANDO_REGISTER_OK      0
#define PANDO_LOGIN_OK         1
#define PANDO_REGISTER_FAIL   -1
#define PANDO_NOT_REGISTERED  -2
#define PANDO_LOGIN_FAIL      -3
#define PANDO_ACCESS_ERR      -4

#endif
