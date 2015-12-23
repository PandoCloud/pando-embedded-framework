#include <stdlib.h>
#include "../../include/pando_timer.h"
#include "os_type.h"
#include "osapi.h"
static os_timer_t timer1;
static os_timer_t timer2;
void FUNCTION_ATTRIBUTE
pando_timer_init(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_disarm(&timer1);
		os_timer_setfn(&timer1, (os_timer_func_t *)timer->timer_cb, timer->arg);
		os_timer_arm(&timer1, timer->interval, timer->repeated);
	}
	if(timer->timer_no==2)
	{
		os_timer_disarm(&timer2);
		os_timer_setfn(&timer2, (os_timer_func_t *)timer->timer_cb, timer->arg);
		os_timer_arm(&timer2, timer->interval, timer->repeated);
	}
}

void FUNCTION_ATTRIBUTE
pando_timer_start(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_arm(&timer1, timer->interval, timer->repeated);
	}
	if(timer->timer_no==2)
	{
		os_timer_arm(&timer2, timer->interval, timer->repeated);
	}
}
void FUNCTION_ATTRIBUTE
pando_timer_stop(struct pd_timer * timer)
{
	if(timer->timer_no==1)
	{
		os_timer_disarm(&timer1);
	}
	if(timer->timer_no==2)
	{
		os_timer_disarm(&timer2);

	}
}

