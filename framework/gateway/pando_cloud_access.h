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

#include "gateway_defs.h"

 /******************************************************************************
 * FunctionName : pando_cloud_access
 * Description  : pando cloud device access api.
 * Parameters   : access_error_callback callback:access callback function
 * Returns      : 
*******************************************************************************/
void pando_cloud_access(gateway_callback callback);

#endif
