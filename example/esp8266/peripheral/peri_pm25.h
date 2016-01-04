/*******************************************************
 * File name: user_plug.h
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

#ifndef __PERI_PM25_H__
#define __PERI_PM25_H__

#define PM_SENT_START1 0x32
#define PM_SENT_START2 0x3d
typedef struct
{
	uint8 start_byte1;
	uint8 start_byte2;
	uint16 length;
	uint16 pm1_0;
	uint16 pm2_5;
	uint16 pm10_0;
	uint16 um0_3;
	uint16 um0_5;
	uint16 um1_0;
	uint16 um2_5;
	uint16 um5_0;
	uint16 um10_0;
	uint16 save[4];
	uint16 crc;
}PM25_REV_DATA;

void display_PMdevice_data(PM25_REV_DATA *data_buffer);
uint16 peri_pm_25_get(void);


#endif

