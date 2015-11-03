/*******************************************************
 * File name: pando_gateway.h
 * Author:
 * Versions:1.0
 * Description:This module is the interface of the pando gateway.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#ifndef __PANDO_GATEWAY_H__
#define __PANDO_GATEWAY_H__

#include "platform/include/pando_types.h"

typedef enum {
    GATEWAY_INIT = 0,
    GATEWAY_REGISTER,
    GATEWAY_LOGIN,
    GATEWAY_ACCESS
}GATEWAY_STATUS;

/******************************************************************************
 * FunctionName : pando_gateway_init
 * Description  : initialize the gateway.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pando_gateway_init(void);

#endif
