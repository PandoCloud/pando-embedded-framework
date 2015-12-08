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
#include "pando_device_register.h"
#include "pando_device_login.h"
#include "pando_cloud_access.h"
#include "../../../util/pando_lan_bind.h"
#include "gateway_defs.h"
#include "../platform/include/pando_sys.h"
#include "pando_zero_device.h"
#include "../platform/include/pando_timer.h"

#include "user_interface.h"
#include "../../../user/wifi_config.h"
#include "espconn.h"

//extern load_data_from_flash();

GATEWAY_STATUS gateway_status;

static void gateway_error_process();
static void gateway_cb(sint8 result); //gateway callback function.
static os_timer_t wifi_check_timer;
/******************************************************************************
 * FunctionName : wifi_connect_check
 * Description  : check the wifi connect status. if device has connected the wifi,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void FUNCTION_ATTRIBUTE
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
		if(get_wifi_config_state() == 1)
		{
			return;
		}
		os_timer_disarm(&wifi_check_timer);
		gateway_status = GATEWAY_LOGIN;
		pando_device_login(gateway_cb);
	}
	else
	{
		PRINTF("WIFI status: not connected\n");
	}
}

/******************************************************************************
 * FunctionName : gateway_cb
 * Description  : the gateway callback function.
 * Parameters   : the gateway flow return value.
 * Returns      : none
*******************************************************************************/
void FUNCTION_ATTRIBUTE
gateway_cb(sint8 result)
{
	PRINTF("Enter into gateway-cb...,gateway_states:%d\n",gateway_status);
	PRINTF("result = %d\n",result);
    switch(gateway_status)
    {
        case GATEWAY_LOGIN:
            if(result == PANDO_NOT_REGISTERED)
            {
            	PRINTF("PANDO_NOT_REGISTERED...\n");
                gateway_status = GATEWAY_REGISTER;
                pando_device_register(gateway_cb);
            }
            else if(result == PANDO_LOGIN_OK)
            {
            	PRINTF("PANDO_LOGIN_OK...\n");
                gateway_status = GATEWAY_ACCESS;
                pando_cloud_access(gateway_cb);
            }
            else
            {
            	PRINTF("else...\n");
                gateway_error_process();
            }
        break;
        case GATEWAY_REGISTER:
            if(result == PANDO_REGISTER_OK)
            {
            	PRINTF("PANDO_REGISTER_OK...\n");
            	pando_device_login(gateway_cb);
            }
            else if (result == PANDO_LOGIN_OK)
            {
            	gateway_status = GATEWAY_LOGIN;
            	pando_cloud_access(gateway_cb);
            }
            else if(result == PANDO_REGISTER_FAIL  )
            {
            	pando_device_register(gateway_cb);
            }
            else if(result == PANDO_LOGIN_FAIL  )
            {
            	pando_device_login(gateway_cb);
            }
            else
            {
            	PRINTF("else2...\n");
            	//pando_device_login(gateway_cb);
            }
        break;
        default:
        	PRINTF("default...\n");
            gateway_error_process();
    }
}

/******************************************************************************
 * FunctionName : wifi_connect_check
 * Description  : check the wifi connect status. if device has connected the wifi,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void FUNCTION_ATTRIBUTE
gprs_connect_check()
{
	PRINTF("Enter into gprs_connect_check...,gateway_states:%d\n",gateway_status);
    gateway_status = GATEWAY_LOGIN;
    pando_device_login(gateway_cb);
}

/******************************************************************************
 * FunctionName : gate_err_process
 * Description  : process the error of gateway.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void FUNCTION_ATTRIBUTE
gateway_error_process()
{
	PRINTF("enter into gateway error !!!");
//    struct pd_timer st_timer;
//   st_timer.interval = 3000;
//    st_timer.repeated = 0;
//    timer1_init(st_timer);
//    timer1_start();
}

/******************************************************************************
 * FunctionName : pando_gateway_init
 * Description  : initialize the gateway.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void FUNCTION_ATTRIBUTE
pando_gateway_init()
{
    pd_printf("PANDO gateway initial....\n");

    gateway_status = GATEWAY_INIT;
    PRINTF("Enter into pando_gateway_init...,gateway_states:%d\n",gateway_status);
    espconn_secure_set_size(ESPCONN_CLIENT,5120);
    load_data_from_flash();

    //pando_system_time_init();
	pando_lan_bind_init();
    pando_zero_device_init();


	os_timer_disarm(&wifi_check_timer);
	os_timer_setfn(&wifi_check_timer, (os_timer_func_t *)wifi_connect_check, NULL);
	os_timer_arm(&wifi_check_timer, 3000, 1);
}
