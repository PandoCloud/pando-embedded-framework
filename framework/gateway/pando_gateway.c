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
#include "platform/include/pando_sys.h"
#include "pando_zero_device.h"
#include "platform/include/pando_timer.h"

//extern load_data_from_flash();

static GATEWAY_STATUS gateway_status;

static void gateway_error_process();

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
                gateway_error_process();
            }
        break;
        case GATEWAY_REGISTER:
            if(result == PANDO_REGISTER_OK)
            {
            	gateway_status = GATEWAY_LOGIN;
                pando_device_login(gateway_cb);
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
 * FunctionName : device_connect_check
 * Description  : check the device connect status. if device has connected the net,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void FUNCTION_ATTRIBUTE
device_connect_check()
{
//	printf("check2\n");
//	if (check_ip_status()== 0)
//	{
//		// device has connected the net
//		timer1_stop();
//		gateway_status = GATEWAY_LOGIN;
//		pando_device_login(gateway_cb);
//	}
//	else
//	{
//		pd_printf("device status: not connected\n");
//	}
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
    struct pd_timer st_timer;
    st_timer.interval = 5000;
    st_timer.repeated = 1;
	st_timer.expiry_cb = device_connect_check;
    timer_init(1, &st_timer);
    timer_start(1);
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

    gateway_status = GATEWAY_REGISTER;

    pando_device_register(gateway_cb);
}
