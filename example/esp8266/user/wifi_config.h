/*******************************************************
 * File name: wifi_config.h
 * Author:Chongguang Li
 * Versions:1.0
 * Description: this module is used to config the wifi ssid and password.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/
#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

 typedef enum {
	CONFIG_TIMEOUT = -1,
	CONFIG_OK
} CONFIG_RESULT;


/*
  callback function after config.
*/
typedef void (* wifi_config_callback)(CONFIG_RESULT result);


/******************************************************************************
 * FunctionName : get_wifi_config_state
 * Description  : get the wifi config state.
 * Parameters   : none.
 * Returns      : 1: the device is in wifi config state.
 * 				  0: the device is not in wifi config state.
*******************************************************************************/
bool get_wifi_config_state();

/******************************************************************************
 * FunctionName : wifi_config
 * Description  : configure the gateway wifi SSID and password.
 * Parameters   : config_cb: the call back function after config, can be NULL.
 * Returns      : none
*******************************************************************************/
void wifi_config(wifi_config_callback config_cb);

#endif
