#include "framework/platform/include/pando_timer.h"
#include <linux/timer.h>

//define the timer reset interval.
#define MUL_TIMER_RESET_SEC 1

typedef struct _timer_manager
{
    struct _timer_info
    {
        int state; /* on or off */
        int repeated; /* repeated or not */
        int interval;
        int elapse; /* 0~interval */
        void (* timer_proc)(void *arg);
        void *func_arg;
    } timer_info[MAX_TIMER_COUNT];

    void (* old_sigfunc)(int);
    void (* new_sigfunc)(int);
    struct itimerval value, ovalue;
} _timer_manager_t;

static struct _timer_manager timer_manager;

static void sig_func(int signo);

/* success, return 0; failed, return -1 */
int init_mul_timer(void)
{
    int ret;
    memset(&timer_manager, 0, sizeof(struct _timer_manager));
    if( (timer_manager.old_sigfunc = signal(SIGALRM, sig_func)) == SIG_ERR)
    {
        return -1;
    }

    timer_manager.new_sigfunc = sig_func;
    timer_manager.value.it_value.tv_sec = MUL_TIMER_RESET_SEC;
    timer_manager.value.it_value.tv_usec = 0;
    timer_manager.value.it_interval.tv_sec = MUL_TIMER_RESET_SEC;
    timer_manager.value.it_interval.tv_usec = 0;
    ret = setitimer(ITIMER_REAL, &timer_manager.value, &timer_manager.ovalue);
    return ret;
}

void timer_init(TIMER_NO timer_no, struct pd_timer *p_timer_cfg)
{
    if(NULL == timer_manager.new_sigfunc)
    {
        return; //means the timer_manager is not initialized.
    }

    if(0 == p_timer_cfg->interval || NULL == p_timer_cfg->expiry_cb)
    {
        return;
    }

    memset(&timer_manager.timer_info[timer_no], 0, sizeof(timer_manager.timer_info[timer_no]));
    timer_manager.timer_info[timer_no].timer_proc = p_timer_cfg->expiry_cb;
    timer_manager.timer_info[timer_no].repeated = p_timer_cfg->repeated;
    timer_manager.tiemr_info[timer_no].interval = p_timer_cfg->interval / 1000; //the mul-timer minimum time unit is set to 1s.
    timer_manager.timer_info[timer_no].elapse = 0;
    timer_manager.timer_info[timer_no].state = 0;
    return;
}

void timer_start(TIMER_NO timer_no)
{
    if(NULL == timer_manager.new_sigfunc)
    {
        return; //means the timer_manager is not initialized.
    }

    timer_manager.timer_info[timer_no].state = 1;
    return;
}

void timer_stop(TIMER_NO timer_no)
{
    if(NULL == timer_manager.new_sigfunc)
    {
        return; //means the timer_manager is not initialized.
    }

    timer_manager.timer_info[timer_no].state = 0;
    return;
}

static void sig_func(int signo)
{
    uint8_t i;
    for(i = 0; i < MAX_TIMER_COUNT; i++)
    {
        if(timer_manager.timer_info[i].state == 0)
        {
            continue;
        }

        timer_manager.timer_info[i].elapse++;
        if(timer_manager.timer_info[i].elapse == timer_manager.timer_info[i].interval)
        {
            timer_manager.timer_info[i].elapse = 0;
            timer_manager.timer_info[i].timer_proc(timer_manager.timer_info[i].func_arg);
            if(timer_manager.tiemr_info[i].repeated == 0)
            {
                timer_manager.timer_info[i].state = 0;
            }
        }
    }

    return;
}
