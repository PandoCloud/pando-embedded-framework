/*******************************************************
 * File name: user_plug.c
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is driver of the subdevice: plug.
               in this example:GPIO 15: the plug relay driver;
                               GPIO 12  a blinking led;
                               GPIO 0   the wifi connect state indicate;
                               GPIO 13  a press key.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "driver/key.h"
#include "driver/uart.h"
#include "driver/uart_register.h"
#include "peri_pm25.h"
#include "driver/tisan_gpio.h"
 
/* NOTICE---this is for i2c communication.
 * 0x84 is the i2c slave device address for 7 bit. */

PM25_REV_DATA *PM25_data_buffer;
extern uint8 RcvBuff[128];
uint16 pm25_data;
void display_PMdevice_data(PM25_REV_DATA *data_buffer)
{
	PRINTF("PM device data display\n");
	PRINTF("start_byte1:%x\n", data_buffer->start_byte1);
	PRINTF("start_byte2:%x\n", data_buffer->start_byte2);
    PRINTF("length:%x\n", data_buffer->length);
	PRINTF("pm1.0:%x\n", data_buffer->pm1_0);
    PRINTF("pm2.5:%x\n", data_buffer->pm2_5);
	PRINTF("pm10:%x\n", data_buffer->pm10_0);
	PRINTF("um0.3:%x\n", data_buffer->um0_3);
	PRINTF("um0.5:%x\n", data_buffer->um0_5);
	PRINTF("um1.0:%x\n", data_buffer->um1_0);
	PRINTF("um2.5:%x\n", data_buffer->um2_5);
	PRINTF("um5.0:%x\n", data_buffer->um5_0);
	PRINTF("um10:%x\n", data_buffer->um10_0);
	ETS_UART_INTR_DISABLE();
}
/******************************************************************************
 * FunctionName : user_mvh3004_read_th
 * Description  : read mvh3004's humiture data
 * Parameters   : uint8 *data - where data to put
 * Returns      : bool - ture or false
********************************************************************************/
uint16 ICACHE_FLASH_ATTR
peri_pm_25_get(void)
{
	uint8 i,data1,data2;
	 if(RcvBuff[0]==0x32)
	 {
		 data1 = RcvBuff[6];
		 data2 = RcvBuff[7];
		 pm25_data = data1*256+ data2;
		 PRINTF("data1=%x , data2=%x\n", data1,data2);
		// PM25_data_buffer->pm2_5 = (RcvBuff[6]<<8)&0xff00 + RcvBuff[7] ;
	 }

	// PRINTF("pm2.5:%x\n", PM25_data_buffer->pm2_5);
	 PRINTF("pm2.5 : %x\n", pm25_data);
	 return pm25_data;
}

