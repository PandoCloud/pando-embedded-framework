#include <stdlib.h>
#include "timer4.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "task.h"

static uint8_t s_repeat_flag;
static time4_callback time_cb;

void timer4_init(uint16_t time_set, uint8_t repeat_flag, time4_callback time4_cb)
{
	s_repeat_flag = repeat_flag;
	time_cb = time4_cb;
    TIM_TimeBaseInitTypeDef time_structure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //enable clock
    time_structure.TIM_Period = 2 * time_set - 1;
    time_structure.TIM_Prescaler = 35999;
    time_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    time_structure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &time_structure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic_structure;
    nvic_structure.NVIC_IRQChannel = TIM4_IRQn; //TIM4 interrupt
    nvic_structure.NVIC_IRQChannelPreemptionPriority = 5;
    nvic_structure.NVIC_IRQChannelSubPriority = 6;
    nvic_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_structure);

    timer4_stop();
}

void timer4_start(void)
{
    TIM_Cmd(TIM4, ENABLE);
}

void timer4_stop(void)
{
    TIM_Cmd(TIM4, DISABLE);
}

void TIM4_IRQHandler(void)
{
	printf("time 4 interrupt!\n");

    //Checks whether the TIM interrupt has occurred or not
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); //Clears the TIMx's interrupt pending bits
        task * timer4_task;
		timer4_task = new_task();
        timer4_task->handler = time_cb;
        timer4_task->pdata = NULL;
        add_task(timer4_task);
        if(0 == s_repeat_flag)
        {
        	printf("time4 stop\n");
            timer4_stop();
        }
    }
}
