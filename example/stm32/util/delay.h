/******************************************************* 
* File name： delay.h
* Author   :  wfq
* Versions :  1.0
* Description: dev delay 
* History: 
*   1.Date:    2014-12-10 
*     Author:  wfq
*     Action:  create
*********************************************************/ 

#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stdint.h>

void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif
