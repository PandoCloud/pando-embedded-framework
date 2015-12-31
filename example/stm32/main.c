#include <stdlib.h>
#include "pando_framework.h"
#include "usart1.h"
#include "usart2.h"
#include "sim5360.h"
#include <stdio.h>
#include "task.h"
#include "delay.h"
#include "malloc.h"
#include "led.h"

char* g_product_key_buf = "d9db7e6a7e11bcd81da5230ad1e3647947f1fb4bf1ff7efa949cea17bab780f3";
char* g_server_url = "https://120.24.222.147";

int main(void)
{
	usart1_init();
	usart2_init();
	led_object_init();
	delay_init();
	mem_init();
	task *task;
	printf("system start\n");
	while(1)
	{		
		if(MODULE_OFF_LINE == get_module_status())
		{
			module_system_start();
		}
		else if(MODULE_SYNC == get_module_status())
		{
			module_system_init();
		}
		else if(MODULE_GET_IP == get_module_status())
		{
			set_module_status(MODULE_INIT_DONE);
			printf("start gateway!\n");
			pando_framework_init();
		}
		while((task = pop_task()) != NULL)
		{
			printf("begin to execute task! %p\r\n", task);
			if(task->handler != NULL)
			{
				task->handler(task->pdata);
			}
			
			if(task != NULL)
			{
				free(task);
				task = NULL;
			}
		}
    }
}
