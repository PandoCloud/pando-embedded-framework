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

typedef enum {
    LOGIN_OK = 0,
    ERR_NOT_REGISTERED,
    ERR_LOGIN_FAIL,
} PANDO_LOGIN_RESULT;

/*
 * "login_callback" is a callback function invoked when device login is finished.
 * "result" returns the register return code.
 */
typedef void (* login_callback)(PANDO_LOGIN_RESULT result);

 /******************************************************************************
 * FunctionName : pando_device_login
 * Description  : try login device using pando cloud device register api.
 * Parameters   : login callback function
 * Returns      : 
*******************************************************************************/
void pando_device_login(login_callback callback);

#endif
