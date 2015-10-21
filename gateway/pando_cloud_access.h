/*******************************************************
 * File name: pando_cloud_access.h
 * Author: ruizeng
 * Versions: 1.0
 * Description:This module is to used to  access to pando cloud.
 * History:
 *   1.Date: initial code
 *     Author: ruizeng
 *     Modification:    
 *********************************************************/
#ifndef __PANDO_CLOUD_ACCESS_H
#define __PANDO_CLOUD_ACCESS_H

typedef enum {
    ACCESS_OK = 0,
    ERR_CONNECT,
    ERR_RECONNECT,
	ERR_DISCONNECT,
	ERR_OTHER
} PANDO_ACCESS_ERROR;

/*
 * "access_error_callback" is a callback function invoked when errors occured accessing pando server.
 * "result" returns the error code.
 */
typedef void (* access_error_callback)(PANDO_ACCESS_ERROR err);

 /******************************************************************************
 * FunctionName : pando_cloud_access
 * Description  : pando cloud device access api.
 * Parameters   : access_error_callback callback:access callback function
 * Returns      : 
*******************************************************************************/
void pando_cloud_access(access_error_callback callback);

#endif
