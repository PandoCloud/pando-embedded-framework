/*******************************************************
 * File name: led.c
 * Author: razr
 * Versions: 1.0
 * Description: the led object
 * History:
 *   1.Date: Sep 15, 2015
 *     Author: razr
 *     Modification:    
 *********************************************************/
#include "pando_object.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "sub_device_protocol.h"


struct led {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

void led_init()
{
	
}

void led_set(struct led* value)
{
	
}

void led_get(struct led* value)
{
	value->red = 255;
	value->green =255;
	value->blue = 255;
}


struct led* create_led()
{
	struct led* led = (struct led*)malloc(sizeof(led));
	return led;
}

void  delete_led(struct led* led)
{
	if(led)
	{
		free(led);
	}
}

void led_object_pack(PARAMS * params)
{
	if(NULL == params)
	{
		printf("Create first tlv param failed.\n");
		return;
	}

	struct led* led = create_led();
	led_get(led);

	if (add_next_uint8(params, led->red))
	{
		printf("Add next tlv param failed.\n");
		return;
	}

	if (add_next_uint8(params, led->green))
	{
		printf("Add next tlv param failed.\n");
		return;
	}

	if (add_next_uint8(params, led->blue))
	{
		printf("Add next tlv param failed.\n");
		return;
	}

	delete_led(led);
}

void led_object_unpack(PARAMS* params)
{
	struct led* led = create_led();

	led->red = get_next_uint8(params);
	led->green = get_next_uint8(params);
	led->blue = get_next_uint8(params);

	led_set(led);

	delete_led(led);

}

void led_object_init()
{
	// init here...
	led_init();

	// register the object
	pando_object led_object = {
		3,
		led_object_pack,
		led_object_unpack,
	};
	register_pando_object(led_object);
}
