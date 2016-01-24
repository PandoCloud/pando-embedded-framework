#include "os_type.h"
#include "osapi.h"
#include "espconn.h"
#include "user_interface.h"

/******************************************************************************
 * FunctionName : wifi_connect_check
 * Description  : check the wifi connect status. if device has connected the wifi,
 * 				  start the gateway flow.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
bool ICACHE_FLASH_ATTR
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
			return 0;
		}
		return 1;
	}
	else
	{
		PRINTF("WIFI status: not connected\n");
		return 0;
	}
}

void ICACHE_FLASH_ATTR
get_device_serial(char* serial_buf)
{
    char device_sta_mac[6];
    wifi_get_macaddr(STATION_IF,device_sta_mac);
    os_sprintf(serial_buf, "%02x%02x%02x%02x%02x%02x", device_sta_mac[0], device_sta_mac[1], device_sta_mac[2], device_sta_mac[3] \
		, device_sta_mac[4], device_sta_mac[5]);
    PRINTF("device_serial:%s\n", serial_buf);
}


