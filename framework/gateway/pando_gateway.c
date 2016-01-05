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
#include "../platform/include/pando_sys.h"
#include "pando_zero_device.h"
#include "../platform/include/pando_timer.h"
#include "../platform/include/platform_miscellaneous_interface.h"

//extern load_data_from_flash();

static GATEWAY_STATUS gateway_status;

static void device_connect_check();

struct pd_timer gateway_timer;

/******************************************************************************
 * FunctionName : gateway_cb
 * Description  : the gateway callback function.
 * Parameters   : the gateway flow return value.
 * Returns      : none
*******************************************************************************/
void FUNCTION_ATTRIBUTE
gateway_cb(int8_t result)
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
            	pando_timer_start(&gateway_timer);
            }
        break;
        case GATEWAY_REGISTER:
            if(result == PANDO_REGISTER_OK)
            {
                pando_cloud_access(gateway_cb);
            }
            else
            {
            	pando_timer_start(&gateway_timer);
            }
        break;
        default:
        	pando_timer_start(&gateway_timer);
    }
}

/******************************************************************************
 * FunctionName : device_connect_check
 * Description  : check wehter the device connect the net. if device has connected the net,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void FUNCTION_ATTRIBUTE
device_connect_check()
{
	if(net_connect_check() == 1)
	{
		gateway_status = GATEWAY_LOGIN;
		pando_timer_stop(&gateway_timer);
		pando_device_login(gateway_cb);
	}
	else
	{
		return;
	}
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

    //load_data_from_flash();

    //pando_system_time_init();

    pando_zero_device_init();

    gateway_timer.interval = 5000;
    gateway_timer.repeated = 1;
    gateway_timer.timer_cb = device_connect_check;
    gateway_timer.timer_no = 2;
    pando_timer_init(&gateway_timer);
    pando_timer_stop(&gateway_timer);
    pando_timer_start(&gateway_timer);
}
