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
#include "pando_gateway.h"
#include "user_interface.h"
#include "pando_device_register.h"
#include "pando_device_login.h"
#include "pando_cloud_access.h"
#include "user_interface.h"
#include "pando_system_time.h"
#include "espconn.h"

typedef enum {
    ERR_LOGIN = 0,
    ERR_REGISTER,
    ERR_ACCESS
}GATEWAY_ERROR;

extern load_data_from_flash();

static os_timer_t wifi_check_timer;

static void login_cb(PANDO_LOGIN_RESULT result); //login callback function.

static void access_error_cb(PANDO_ACCESS_ERROR err); //access error callback function.

static void register_cb(PANDO_REGISTER_RESULT result); // register callback function.


/******************************************************************************
 * FunctionName : wifi_connect_check
 * Description  : check the wifi connect status. if device has connected the wifi,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
wifi_connect_check()
{
	struct ip_info device_ip;
	uint8 connect_status = 0;
	wifi_get_ip_info(STATION_IF, &device_ip);
	connect_status = wifi_station_get_connect_status();
	if (connect_status == STATION_GOT_IP && device_ip.ip.addr != 0)
	{
		// device has connected the wifi.

		// the device is in wifi config mode, waiting for wifi config mode over.
		if(get_wifi_conifg_state() == 1)
		{
			return;
		}
		os_timer_disarm(&wifi_check_timer);
		pando_device_login(login_cb);
	}
	else
	{
		PRINTF("WIFI status: not connected\n");
	}
}

/******************************************************************************
 * FunctionName : gate_err_process
 * Description  : process the error of gateway.
 * Parameters   : the login result.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
gateway_err_process(GATEWAY_ERROR err)
{
	os_timer_disarm(&wifi_check_timer);
	os_timer_setfn(&wifi_check_timer, (os_timer_func_t *)wifi_connect_check, NULL);
	os_timer_arm(&wifi_check_timer, 3000, 1);
}

/******************************************************************************
 * FunctionName : login_cb
 * Description  : the callback function after device login pando cloud.
 * Parameters   : the login result.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
login_cb(PANDO_LOGIN_RESULT result)
{
    PRINTF("PANDO login result: %d\n", result);
    
    if(result == LOGIN_OK)
    {
        pando_cloud_access(access_error_cb);

    }
    else if(result == ERR_NOT_REGISTERED)
    {
        pando_device_register(register_cb);
    }
    else
    {
    	gateway_err_process(ERR_LOGIN);
    }
}

/******************************************************************************
 * FunctionName : register_cb
 * Description  : the callback function after device register pando cloud.
 * Parameters   : the register result.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
register_cb(PANDO_REGISTER_RESULT result)
{
    PRINTF("PANDO register result: %d\n", result);

    if(result == REGISTER_OK)
    {
    	pando_device_login(login_cb);
    }
    else
    {
    	gateway_err_process(ERR_REGISTER);
    }
}

/******************************************************************************
 * FunctionName : access_error_cb
 * Description  : the callback function when device access pando cloud wrong.
 * Parameters   : the access error cause.
 * Returns      : none
*******************************************************************************/
static void ICACHE_FLASH_ATTR
access_error_cb(PANDO_ACCESS_ERROR err)
{
    PRINTF("PANDO access error: %d\n", err);

    gateway_err_process(ERR_ACCESS);

}

/******************************************************************************
 * FunctionName : pando_gateway_init
 * Description  : initialize the gateway.
 * Parameters   : gateway_err_callback err_cb : the callback function when gateway \
 * 				  connect pando cloud error.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
pando_gateway_init()
{
    PRINTF("PANDO gateway initial....\n");

    espconn_secure_set_size(ESPCONN_CLIENT,5120);

    load_data_from_flash();

	pando_system_time_init();

	pando_lan_bind_init();

	pando_zero_device_init();

	os_timer_disarm(&wifi_check_timer);
	os_timer_setfn(&wifi_check_timer, (os_timer_func_t *)wifi_connect_check, NULL);
	os_timer_arm(&wifi_check_timer, 3000, 1);
}

