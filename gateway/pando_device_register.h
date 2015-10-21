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

typedef enum {
    REGISTER_OK = 0,
    ERR_REGISTER_FAIL
} PANDO_REGISTER_RESULT;

/*
 * "register_callback" is a callback function invoked when device register is finished.
 * "result" returns the register return code.
 */
typedef void (* register_callback)(PANDO_REGISTER_RESULT result);

 /******************************************************************************
 * FunctionName : pando_device_register
 * Description  : register pando device using pando cloud device register api.
 * Parameters   : register callback function
 * Returns      : 
*******************************************************************************/
void pando_device_register(register_callback callback);

#endif
