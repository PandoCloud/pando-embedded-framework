#include <stdlib.h>
#include "platform/include/pando_timer.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "task.h"

struct pd_timer *g_timer3_config = NULL;
struct pd_timer *g_timer2_config = NULL;

void timer3_init(struct pd_timer * timer);
void timer3_start(void);
void timer3_stop(void);
void timer2_init(void);
void timer2_start(void);
void timer2_stop(void);

void pando_timer_init(struct pd_timer * timer)
{
	if(timer->timer_no == 3)
	{
		g_timer3_config = timer;
		timer3_init(timer);
	}
	else if(timer->timer_no == 2)
	{
		g_timer2_config = timer;
		timer2_init();
	}
	else
	{
		printf("wrong timer no!\n");
	}
}

void pando_timer_start(struct pd_timer * timer)
{
	if(timer->timer_no == 3)
	{
		timer3_start();

	}
	else if(timer->timer_no == 2)
	{
		timer2_start();
	}
	else
	{
		printf("wrong timer no!\n");
	}
}


void pando_timer_stop(struct pd_timer * timer)
{
	if(timer->timer_no == 3)
	{
		timer3_stop();

	}
	else if(timer->timer_no == 2)
	{
		timer2_stop();
	}
	else
	{
		printf("wrong timer no!\n");
	}
}

void timer3_init(struct pd_timer * timer)
{
    TIM_TimeBaseInitTypeDef time_structure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //enable clock
    time_structure.TIM_Period = 2 * (timer->interval) - 1;
    time_structure.TIM_Prescaler = 35999;
    time_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    time_structure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &time_structure);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic_structure;
    nvic_structure.NVIC_IRQChannel = TIM3_IRQn; //TIM2 interrupt
    nvic_structure.NVIC_IRQChannelPreemptionPriority = 5;
    nvic_structure.NVIC_IRQChannelSubPriority = 6;
    nvic_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_structure);

    timer3_stop();
}

void timer3_start(void)
{
    TIM_Cmd(TIM3, ENABLE);
}

void timer3_stop(void)
{
    TIM_Cmd(TIM3, DISABLE);
}

void TIM3_IRQHandler(void)
{
	printf("timer 3 interrupt!\n");
    //Checks whether the TIM interrupt has occurred or not
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //Clears the TIMx's interrupt pending bits
        if(NULL != g_timer3_config->timer_cb)
        {
            g_timer3_config->timer_cb(g_timer3_config->arg);
        }
        task * timer3_task;
        timer3_task = new_task();
        timer3_task->handler = g_timer3_config->timer_cb;
        timer3_task->pdata = g_timer3_config->arg;
        add_task(timer3_task);

        if(0 == g_timer3_config->repeated)
        {
            timer3_stop();
        }
    }
}

void timer2_init(void)
{
    TIM_TimeBaseInitTypeDef time_structure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //enable clock
    time_structure.TIM_Period = 2 * (g_timer2_config->interval) - 1;
    time_structure.TIM_Prescaler = 35999;
    time_structure.TIM_ClockDivision = TIM_CKD_DIV1;
    time_structure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &time_structure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic_structure;
    nvic_structure.NVIC_IRQChannel = TIM2_IRQn; //TIM2 interrupt
    nvic_structure.NVIC_IRQChannelPreemptionPriority = 5;
    nvic_structure.NVIC_IRQChannelSubPriority = 6;
    nvic_structure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_structure);

    timer2_stop();
}


void timer2_start(void)
{
    TIM_Cmd(TIM2, ENABLE);
}

void timer2_stop(void)
{
    TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQHandler(void)
{
	printf("timer 2 interrupt!\n");
    //Checks whether the TIM interrupt has occurred or not
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //Clears the TIMx's interrupt pending bits

        task * timer2_task;
        timer2_task = new_task();
        timer2_task->handler = g_timer2_config->timer_cb;
        timer2_task->pdata = g_timer2_config->arg;
        add_task(timer2_task);

        if(0 == g_timer2_config->repeated)
        {
            timer2_stop();
        }
    }
}
