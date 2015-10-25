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
#include "gateway_defs.h"
#include "../../peripherl/driver/timer2.h"

extern load_data_from_flash();

static os_timer_t wifi_check_timer;
static GATEWAY_STATUS gateway_status;
/******************************************************************************
 * FunctionName : gateway_cb
 * Description  : the gateway callback function.
 * Parameters   : the gateway flow return value.
 * Returns      : none
*******************************************************************************/
void gateway_cb(sint8 result)
{
	switch(gateway_status)
	{
		case GATEWAY_LOGIN:
			if(result == PANDO_NOT_REGISTERED)
			{
				gateway_status = GATEWAY_REGISTER;
				pando_device_register(gateway_cb);
			}
			else if(result == PANDO_LOGIN_OK)
			{
				gateway_status = GATEWAY_ACCESS;
				pando_cloud_access(gateway_cb);
			}
			else
			{
				gateway_error_process();
			}
		break;
		case GATEWAY_REGISTER:
			if(result == PANDO_REGISTER_OK)
			{
				pando_cloud_access(gateway_cb);
			}
			else
			{
				gateway_error_process();
			}
		break;
		default:
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
static void gprs_connect_check()
{
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
	timer2_init(3, gprs_connect_check);
	timer2_start();
}


/******************************************************************************
 * FunctionName : pando_gateway_init
 * Description  : initialize the gateway.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void pando_gateway_init()
{
    PD_LOG("PANDO gateway initial....\n");

    gateway_status = GATEWAY_INIT;

    load_data_from_flash();

	pando_system_time_init();

	pando_zero_device_init();

	os_timer_disarm(&wifi_check_timer);
	os_timer_setfn(&wifi_check_timer, (os_timer_func_t *)gprs_check, NULL);
	os_timer_arm(&wifi_check_timer, 3000, 1);
}

