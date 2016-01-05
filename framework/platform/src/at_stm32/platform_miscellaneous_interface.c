#include "../include/pando_sys.h"
#include "../include/pando_types.h"
#include "sim5360.h"

extern uint8_t g_imei_buf[16];

/******************************************************************************
 * FunctionName : net_connect_check
 * Description  : check whether the platform connect the net.
 * Parameters   : none.
 * Returns      : 1: the platform connect the net OK.
 * 				  0: the platform dose not connect the net.
*******************************************************************************/
bool net_connect_check(void)
{
	// TODO: the acquired signal quality is not the current value, but the last inquired value.
	uint8_t signal_quality = 0;
	signal_quality = inquire_signal_quality();
	if( signal_quality> MIN_SIGNAL_QUAILTY)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
 * FunctionName : get_device_serial
 * Description  : get the unique identification of the device, such as mac address of internet device or imei of mobile device.
 * Parameters   : serial_buf: the buf storage the serial.
 * Returns      : none.
*******************************************************************************/
void get_device_serial(char* serial_buf)
{
	pd_memcpy(serial_buf, g_imei_buf, sizeof(g_imei_buf));
}


