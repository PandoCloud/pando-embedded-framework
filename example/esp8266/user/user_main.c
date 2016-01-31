/******************************************************************************
 /*******************************************************
 * File name: user_main.c
 * Author:Chongguang Li
 * Versions:1.0
 * Description:entry file of user application.
 *
 * History:
 *   1.Date:
 *     Author:
 *     Modification:
*******************************************************************************/

#include "../pando/framework/pando_framework.h"
#include "../peripheral/driver/uart.h"
#include "objects/led.h"
#include "os_type.h"
#include "user_interface.h"
#include "../peripheral/peri_key.h"
#include "wifi_config.h"
#include "espconn.h"

char* g_product_key_buf = "d0375b4180f1ddc44c89a2046e8b841a7d2b642303a026bfc101aa0e448b6dee";
//#define SERVER_ADDRESS "https://api.pandocloud.com"
char* g_server_url = "https://120.24.222.147";
//char* g_server_url = "https://api.pandocloud.com";
static void ICACHE_FLASH_ATTR
user_key_long_press_cb()
{
	PRINTF("Long press occurred");
	wifi_config(NULL);
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
	uart_init(115200, 115200); // serial bound rate:11520.

	//long press gpio4, enter into wifi  config mode.
	peri_single_key_init(4, user_key_long_press_cb, peri_key_short_press);

	// add you object init here.
	led_object_init();
	//c_motor_object_init();
	//temperature_object_init();

	espconn_secure_set_size(ESPCONN_CLIENT,5120);
	pando_framework_init();
}

