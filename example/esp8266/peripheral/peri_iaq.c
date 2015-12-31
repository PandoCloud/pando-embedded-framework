/*******************************************************
 * File name: peri_iaq5000.c
 * Author: Shasha Liu
 * Versions:1.0
 * Description:This module is driver of the subdevice: Indoor air quality detect module.
               in this example:GPIO 15: the plug relay driver;
                               GPIO 12  a blinking led;
                               GPIO 0   the wifi connect state indicate;
                               GPIO 13  a press key.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:    
 *********************************************************/
 
#include "peri_iaq.h"

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"
#include "driver/key.h"
#include "driver/i2c.h"
#include "driver/tisan_gpio.h"
 
/* NOTICE---this is for i2c communication.
 * 0x84 is the i2c slave device address for 7 bit. */
#define IAQ5000_ADD		  0x84
#define IAQ5000_ADD_W     0x84
#define IAQ5000_ADD_R     0x85

/******************************************************************************
 * FunctionName : user_mvh3004_burst_read
 * Description  : burst read mvh3004's internal data
 * Parameters   : uint8 addr - mvh3004's address
 *                uint8 *pData - data point to put read data
 *                uint16 len - read length
 * Returns      : bool - true or false
*******************************************************************************/
LOCAL bool ICACHE_FLASH_ATTR
peri_iaq_single_burst_read(uint8 addr, uint8 *pData, uint16 len)
{
    uint8 ack;
    uint16 i;

    i2c_start();
    i2c_writeByte(addr);
    ack = i2c_check_ack();
    PRINTF("the first ack is:%d\n",ack);
    if (ack==0) {
        os_printf("addr1 not ack when tx write cmd \n");
        i2c_stop();
        return false;
    }


      i2c_writeByte(0x52);
      ack = i2c_check_ack();
      PRINTF("the second ack is:%d\n",ack);

      if (ack==0) {
          os_printf("not ack when write 0x52 \n");
          i2c_stop();
          return false;
      }

    i2c_start();
    i2c_writeByte(addr + 1);
    ack = i2c_check_ack();
    PRINTF("the third ack is:%d\n",ack);
    if (ack==0) {
        os_printf("addr2 not ack when tx write cmd \n");
        i2c_stop();
        return false;
    }
    os_delay_us(1);
    for (i = 0; i < len; i++) {
        pData[i] = i2c_readByte();
        if(i==3)
            i2c_send_ack(0);
        else
        	i2c_send_ack(1);


        os_delay_us(1);
    	PRINTF("bytes_readed:%d\n", pData[i]);
    }

    i2c_stop();


    return true;
}
/******************************************************************************
 * FunctionName : user_mvh3004_read_th
 * Description  : read mvh3004's humiture data
 * Parameters   : uint8 *data - where data to put
 * Returns      : bool - ture or false
*******************************************************************************/
uint16 ICACHE_FLASH_ATTR
peri_iaq_read(void)
{
	uint8 data[4];
	uint16 iaq_value;
	peri_iaq_single_burst_read(IAQ5000_ADD,data, 4);

    if((data[0]!=0xA5)||(data[3]!=0x3C))
    {
    	os_printf("iaq5000 read data error \n");
    	return 0;
    }
    else
    {
    	iaq_value = data[1]*256+data[2];
    	os_printf("iaq5000 read data :%d \n",iaq_value);
    	return iaq_value;

    }

}
/******************************************************************************
 * FunctionName : user_iaq5000_init
 * Description  : init iaq5000, mainly i2c master gpio
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_iaq_init(void)
{
    i2c_init();
}
