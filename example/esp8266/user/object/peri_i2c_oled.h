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

#ifndef APP_PERIPHERAL_PERI_I2C_OLED_H_
#define APP_PERIPHERAL_PERI_I2C_OLED_H_

#include "c_types.h"

#define OLED_ADDRESS	0x78  // D/C->GND
//#define OLED_ADDRESS	0x7a // D/C->Vcc

/******************************************************************************
 * FunctionName : peri_i2c_oled_init
 * Description  : initialize the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
bool ICACHE_FLASH_ATTR
peri_i2c_oled_init(void);

/******************************************************************************
 * FunctionName : peri_oled_clean_screen.
 * Description  : clean the screen of the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_clean_screen(void);

/******************************************************************************
 * FunctionName : peri_oled_on.
 * Description  : turn on the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_on(void);

/******************************************************************************
 * FunctionName : peri_oled_off.
 * Description  : turn off the oled.
 * Parameters   : none.
 * Returns      : none.
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_oled_off(void);

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
peri_oled_show_str(unsigned char x, unsigned char y, unsigned char ch[], unsigned char len, unsigned char text_size);
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
peri_oled_draw_bmp(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char bmp[]);
#endif /* APP_PERIPHERAL_PERI_I2C_OLED_H_ */
