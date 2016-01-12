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

char* g_product_key_buf = "d0375b4180f1ddc44c89a2046e8b841a7d2b642303a026bfc101aa0e448b6dee";
//char* g_product_key_buf = "fedf2397677f5a03e4cd0e18994e54aeeada7a4fe514bcbaf03c4d2c6f19317e" ;
//char* g_product_key_buf = "b4d967f08c43636cf78c1c6bb1e5835a1795483802b66826f853fe5640ef16ab";

char* g_server_url = "https://120.24.222.147";
//char* g_server_url = "https://api.pandocloud.com";

void delay_test(u16 time)
{
    u16 i,j;
    for(i=0; i<time; i++)
    {
        for(j=0; j < 100 ; j++);
    }
}

int main(void)
{
    delay_init();
	usart1_init();
	usart2_init();
	led_object_init();	
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
