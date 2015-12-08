#include <stdlib.h>
#include "../../include/pando_timer.h"
#include "os_type.h"
#include "osapi.h"

static os_timer_t timer1;
void FUNCTION_ATTRIBUTE
timer1_init(struct pd_timer timer_cfg)
{
	os_timer_disarm(&timer1);
	os_timer_setfn(&timer1, (os_timer_func_t *)timer_cfg.timer_cb, timer_cfg.arg);
	os_timer_arm(&timer1, timer_cfg.interval, timer_cfg.repeated);

}

void FUNCTION_ATTRIBUTE
timer1_start(void)
{
	;
}
void FUNCTION_ATTRIBUTE
timer1_stop(void)
{
;	//os_timer_disarm(&timer1);
}

