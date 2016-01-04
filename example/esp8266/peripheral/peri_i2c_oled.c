/*******************************************************
 * File name: peri_i2c_oled.h
 * Author: Chongguang Li
 * Versions:1.0
 * Description:This module is the i2c oled driver.
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
 *********************************************************/

#include "driver/i2c.h"
#include "driver/tisan_gpio.h"
#include "peri_i2c_oled.h"
#include "osapi.h"
#include "mem.h"
#include "oled_font.h"
#include "user_interface.h"

#define MAX_SMALL_FONTS 21 //SMALL FONTS
#define MAX_BIG_FONTS 16 //BIG FONTS

static bool oledstat = false;

/******************************************************************************
 * FunctionName : oled_write_reg
 * Description  : write the register of the i2c oled.
 * Parameters   : uint8_t reg_addr: the register address.
 * 				  uint8_t val: the write value.
 * Returns      : bool : the write result, 0:failed, 1:success.
*******************************************************************************/
static bool ICACHE_FLASH_ATTR
oled_write_reg(uint8_t reg_addr,uint8_t val)
{
  i2c_start();
  i2c_writeByte(OLED_ADDRESS);
  if(!i2c_check_ack())
  {
    i2c_stop();
    return 0;
  }
  i2c_writeByte(reg_addr);
  if(!i2c_check_ack())
  {
    i2c_stop();
    return 0;
  }
  i2c_writeByte(val&0xff);
  if(!i2c_check_ack())
  {
    i2c_stop();
    return 0;
  }
  i2c_stop();

  if(reg_addr==0x00)
  oledstat=true;

  return 1;
}

/******************************************************************************
 * FunctionName : oled_write_cmd
 * Description  : write the command to the oled.
 * Parameters   : unsigned char i2c_command: the command
 * Returns      : none.
*******************************************************************************/
static void ICACHE_FLASH_ATTR
oled_write_cmd(unsigned char i2c_command)
{
  oled_write_reg(0x00, i2c_command);
}

/******************************************************************************
 * FunctionName : oled_write_data
 * Description  : write the data to the oled.
 * Parameters   : unsigned char i2c_data: the write data.
 * Returns      : none.
*******************************************************************************/
static void ICACHE_FLASH_ATTR
oled_write_data(unsigned char i2c_data)
{
	oled_write_reg(0x40,i2c_data);
}

/******************************************************************************
 * FunctionName : peri_i2c_oled_init
 * Description  : initialize the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
bool ICACHE_FLASH_ATTR
peri_i2c_oled_init(void)
{
	i2c_init();
	os_delay_us(60000);
	os_delay_us(40000);

	oled_write_cmd(0xae); // turn off oled panel
	oled_write_cmd(0x00); // set low column address
	oled_write_cmd(0x10); // set high column address
	oled_write_cmd(0x40); // set start line address
	oled_write_cmd(0x81); // set contrast control register

	oled_write_cmd(0xa0);
	oled_write_cmd(0xc0);

	oled_write_cmd(0xa6); // set normal display
	oled_write_cmd(0xa8); // set multiplex ratio(1 to 64)
	oled_write_cmd(0x3f); // 1/64 duty
	oled_write_cmd(0xd3); // set display offset
	oled_write_cmd(0x00); // not offset
	oled_write_cmd(0xd5); // set display clock divide ratio/oscillator frequency
	oled_write_cmd(0x80); // set divide ratio
	oled_write_cmd(0xd9); // set pre-charge period
	oled_write_cmd(0xf1);
	oled_write_cmd(0xda); // set com pins hardware configuration
	oled_write_cmd(0x12);
	oled_write_cmd(0xdb); // set vcomh
	oled_write_cmd(0x40);
	oled_write_cmd(0x8d); // set Charge Pump enable/disable
	oled_write_cmd(0x14); // set(0x10) disable
	oled_write_cmd(0xaf); // turn on oled panel

	peri_oled_clean_screen();  //OLED CLS

	return oledstat;
}

static void ICACHE_FLASH_ATTR
oled_set_position(unsigned char x, unsigned char y)
{
	oled_write_cmd(0xb0+y);
	oled_write_cmd(((x&0xf0)>>4)|0x10);
	oled_write_cmd((x&0x0f)|0x01);
}

/******************************************************************************
 * FunctionName : peri_oled_clean_screen.
 * Description  : clean the screen of the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_clean_screen(void)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		oled_write_cmd(0xb0+m);		//page0-page1
		oled_write_cmd(0x00);		//low column start address
		oled_write_cmd(0x10);		//high column start address
		for(n=0;n<132;n++)
		{
			oled_write_data(0);
		}
	}
}

/******************************************************************************
 * FunctionName : peri_oled_on.
 * Description  : turn on the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_on(void)
{
	oled_write_cmd(0X8D);
	oled_write_cmd(0X14);
	oled_write_cmd(0XAF);
}

/******************************************************************************
 * FunctionName : peri_oled_off.
 * Description  : turn off the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_off(void)
{
	oled_write_cmd(0X8D);
	oled_write_cmd(0X10);
	oled_write_cmd(0XAE);
}

/******************************************************************************
 * FunctionName : peri_oled_show_str.
 * Description  : print string to the oled.
 * Parameters   : unsigned char x: the string x position;
 * 				  unsigned char y: the string y position;
 * 				  unsigned char ch[]: the print string;
 * 				  unsigned char len: the length of the string.
 * 				  unsigned char text_size: the size of the string.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_show_str(unsigned char x, unsigned char y, unsigned char ch[], unsigned char len, unsigned char text_size)
{
	unsigned char c = 0,i = 0,j = 0, text_len = 0, str_len;
	text_len = len;
	str_len = os_strlen(ch);
	switch(text_size)
	{
		case 1:
		{
			for(; j < len; )
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				if(j <= str_len)
				{
					oled_set_position(x,y);
					for(i=0;i<6;i++)
					{
						oled_write_data(F6x8[c][i]);
					}
					x += 6;
					j++;
				}

				else
				{
					oled_set_position(x,y);
					for(i=0;i<6;i++)
					{
						oled_write_data(F6x8[0][i]);
				    }
					x += 6;
					j++;
				}
			}
		}
		break;
		case 2:
		{
			for(; j <len; )
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}

				if(j <= str_len)
				{
					oled_set_position(x,y);
					for(i=0;i<8;i++)
					oled_write_data(F8X16[c*16+i]);
					oled_set_position(x,y+1);
					for(i=0;i<8;i++)
					oled_write_data(F8X16[c*16+i+8]);
					x += 8;
					j++;
				}
				else
				{
					oled_set_position(x,y);
					for(i=0;i<8;i++)
					oled_write_data(F8X16[0*16+i]);
					oled_set_position(x,y+1);
					for(i=0;i<8;i++)
					oled_write_data(F8X16[0*16+i+8]);
					x += 8;
					j++;
				}
			}
		}
		break;
	}
}

/******************************************************************************
 * FunctionName : peri_oled_draw_bmp.
 * Description  : print the bmp picture to the oled.
 * Parameters   : unsigned char x0: the left horizontal position of the bmp picture;
 * 				  unsigned char y0: the left vertical position of the bmp picture;
 * 				  unsigned char x1: the right horizontal position of the bmp picture;
 * 				  unsigned char y1: the left vertical position of the bmp picture;
 * 				  unsigned char bmp[]: the bmp picture array;
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_draw_bmp(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char bmp[])
{
	unsigned int j=0;
	unsigned char x,y;
	if(y1%8==0)
	{
		y = y1/8;
	}
	else
	{
		y = y1/8 + 1;
	}
	for(y=y0;y<y1;y++)
	{
		oled_set_position(x0,y);
		for(x=x0;x<x1;x++)
		{
			oled_write_data(bmp[j++]);
		}
	}
}
