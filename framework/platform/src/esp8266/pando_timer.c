#include <stdlib.h>
#include "../../include/pando_timer.h"
#include "os_type.h"
#include "osapi.h"

static os_timer_t timer1;
void ICACHE_FLASH_ATTR
timer1_init(struct pd_timer timer_cfg)
{

	os_timer_disarm(&timer1);
	os_timer_setfn(&timer1, (os_timer_func_t *)timer_cfg.expiry_cb, NULL);
	os_timer_arm(&timer1, timer_cfg.interval, timer_cfg.repeated);

}

void ICACHE_FLASH_ATTR
timer1_start(void)
{
	;
}
void ICACHE_FLASH_ATTR
timer1_stop(void)
{
	os_timer_disarm(&timer1);
}

