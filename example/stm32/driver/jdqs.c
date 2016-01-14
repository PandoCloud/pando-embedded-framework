
#include "cm3_bitband.h"
#include "delay.h"
#include "jdqs.h"

#define GPIO_JDQ1 PBout(6)  

void jdq_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// Enable clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

void set_jdq_on(void)
{
    GPIO_JDQ1 = 1;
}

void set_jdq_off(void)
{
    GPIO_JDQ1 = 0;
}

void togle_jdq(void)
{
    GPIO_JDQ1 = !GPIO_JDQ1;
}

void set_jdq(char state)
{
    if(state == 0)
    {
        GPIO_JDQ1 = 0;
    }
    else
    {
        GPIO_JDQ1 = 1;
    }
}

