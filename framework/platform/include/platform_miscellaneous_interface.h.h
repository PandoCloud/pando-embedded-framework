/*******************************************************
 * File name: platform_misceelaneous_interface.h
 * Author: Chongguang Li
 * Versions: 1.0
 * Description:This module includes platform relationship api.
 * History:
 *   1.Date: initial code
 *     Author: Chongguang Li
 *     Modification:    
 *********************************************************/
#ifndef _PLATFORM_MISCEELANEOUS_INTERFACE_H
#define _PLATFORM_MISCEELANEOUS_INTERFACE_H

/******************************************************************************
 * FunctionName : net_connect_check
 * Description  : check whether the platform connect the net.
 * Parameters   : none.
 * Returns      : 1: the platform connect the net OK.
 * 				  0: the platform dose not connect the net.
*******************************************************************************/
bool net_connect_check(void);

/******************************************************************************
 * FunctionName : get_device_serial
 * Description  : get the unique identification of the device, such as mac address of internet device or imei of mobile device.
 * Parameters   : serial_buf: the buf storage the serial.
 * Returns      : none.
*******************************************************************************/
void get_device_serial(char* serial_buf);



#endif
