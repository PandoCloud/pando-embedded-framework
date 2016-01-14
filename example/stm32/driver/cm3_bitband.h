
#ifndef _CM3_BITBAND_H_
#define _CM3_BITBAND_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

//bit band operation, just like 8051 how to operate GPIO
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO address map
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO operation,single IO!
//make sure n less than 16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //output 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //input 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //output 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //input 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //output 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //input 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //output 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //input 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //output 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //input

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //output 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //input

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //output 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //input

#endif
