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
	if(inquire_signal_quality() > 15)
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
	strcpy(serial_buf, g_imei_buf);
}


