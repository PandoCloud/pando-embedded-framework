#include <stdlib.h>
#include "../../include/pando_timer.h"
#include "../../../../../user/wifi_config.h"
#include "os_type.h"
#include "osapi.h"
#include "espconn.h"
#include "user_interface.h"
//#include "../../../gateway/pando_gateway.h"
static os_timer_t timer1;
static os_timer_t timer2;
void FUNCTION_ATTRIBUTE
pando_timer_init(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_disarm(&timer1);
		os_timer_setfn(&timer1, (os_timer_func_t *)timer->timer_cb, timer->arg);
		os_timer_arm(&timer1, timer->interval, timer->repeated);
	}
	if(timer->timer_no==2)
	{
		os_timer_disarm(&timer2);
		os_timer_setfn(&timer2, (os_timer_func_t *)timer->timer_cb, timer->arg);
		os_timer_arm(&timer2, timer->interval, timer->repeated);
	}
}

void FUNCTION_ATTRIBUTE
pando_timer_start(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_arm(&timer1, timer->interval, timer->repeated);
	}
	if(timer->timer_no==2)
	{
		os_timer_arm(&timer2, timer->interval, timer->repeated);
	}
}
void FUNCTION_ATTRIBUTE
pando_timer_stop(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_disarm(&timer1);
	}
	if(timer->timer_no==2)
	{
		os_timer_disarm(&timer2);

	}
}
/******************************************************************************
 * FunctionName : wifi_connect_check
 * Description  : check the wifi connect status. if device has connected the wifi,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
NET_CONNECT_STATUS ICACHE_FLASH_ATTR
net_connect_check(void)
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
			return NET_CONNECT_CONNECTING;
		}
		return NET_CONNECT_OK;
	}
	else
	{
		PRINTF("WIFI status: not connected\n");
		return NET_CONNECT_FAIL;
	}
}

