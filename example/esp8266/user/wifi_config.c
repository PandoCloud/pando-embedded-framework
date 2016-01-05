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

#include "user_interface.h"
#include "smartconfig.h"
#include "wifi_config.h"
#include "../peripheral/peri_led.h"

static wifi_config_callback wifi_config_cb = NULL;
static bool s_config_start_flag = 0;


/******************************************************************************
 * FunctionName : get_wifi_config_state
 * Description  : get the wifi config state.
 * Parameters   : none.
 * Returns      : 1: the device is in wifi config state.
 * 				  0: the device is not in wifi config state.
*******************************************************************************/
bool ICACHE_FLASH_ATTR
get_wifi_config_state()
{
	return s_config_start_flag;
}

static void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
	switch(status)
	{
		case SC_STATUS_WAIT:
		os_printf("SC_STATUS_WAIT\n");
		break;

		case SC_STATUS_FIND_CHANNEL:
		os_printf("SC_STATUS_FIND_CHANNEL\n");
		break;

		case SC_STATUS_GETTING_SSID_PSWD:
		os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
		sc_type *type = pdata;
		if (*type == SC_TYPE_ESPTOUCH)
		{
			os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
		}
		else
		{
			os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
		}
		break;

		case SC_STATUS_LINK:
		os_printf("SC_STATUS_LINK\n");
		peri_led_set(0); // led indicate the device has got ssid and password.
		struct station_config *sta_conf = pdata;
		wifi_station_set_config(sta_conf);
		wifi_station_disconnect();
		wifi_station_connect();
		break;

		case SC_STATUS_LINK_OVER:
	    os_printf("SC_STATUS_LINK_OVER\n");
	    if (pdata != NULL)
		{
	    	uint8 phone_ip[4] = {0};
	    	os_memcpy(phone_ip, (uint8*)pdata, 4);
	    	os_printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
		}
	    peri_led_set(1); // led indicate the wifi config over.
		smartconfig_stop();
		s_config_start_flag = 0;
		if(wifi_config_cb != NULL)
		{
			wifi_config_cb(CONFIG_OK);
		}
        break;
    }

}



/******************************************************************************
 * FunctionName : wifi_config
 * Description  : config the wifi SSID and password.
 * Parameters   : config_cb:the call back function after config.
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
wifi_config(wifi_config_callback config_cb)
{
	PRINTF("wifi config start...\n");

	// gpio5 drive the led, which indicate the wifi config mode.
	peri_led_init(5);
	peri_led_blink(BLINK_QUICK);

	wifi_config_cb = config_cb;
    if(s_config_start_flag)
    {
    	smartconfig_stop();
    }

    s_config_start_flag = 1;
    smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);
    wifi_set_opmode(STATION_MODE);
    smartconfig_start(smartconfig_done);
}
