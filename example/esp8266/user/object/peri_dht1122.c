/*
    Driver for the temperature and humidity sensor DHT11 and DHT22
    Official repository: https://github.com/CHERTS/esp8266-dht11_22

    Copyright (C) 2014 Mikhail Grigorev (CHERTS)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ets_sys.h"
#include "eagle_soc.h"
#include "osapi.h"
#include "c_types.h"
#include "user_interface.h"
#include "driver/gpio.h"
#include "driver/tisan_gpio.h"
#include "peri_dht1122.h"

static DHT_Sensor dht_sensor;

#define sleepms(x) os_delay_us(x*1000);
/******************************************************************************
 * FunctionName : scale_humidity
 * Description  : get humidity value
 * Parameters   : DHTType sensor_type, int *data
 * Returns      : humidity
*******************************************************************************/
static inline
float scale_humidity(DHTType sensor_type, int *data)
{
	if(sensor_type == DHT11)
	{
		return (float) data[0];
	}
	else
	{
		float humidity = data[0] * 256 + data[1];
		return humidity /= 10;
	}
}
/******************************************************************************
 * FunctionName : scale_temperature
 * Description  : get scale_temperature value
 * Parameters   : DHTType sensor_type, int *data
 * Returns      : temperature
*******************************************************************************/
static inline
float scale_temperature(DHTType sensor_type, int *data)
{
	if(sensor_type == DHT11)
	{
		return (float) data[2];
	}
	else
	{
		float temperature = data[2] & 0x7f;
		temperature *= 256;
		temperature += data[3];
		temperature /= 10;
		if (data[2] & 0x80)
			temperature *= -1;
		return temperature;
	}
}
/******************************************************************************
 * FunctionName : DHTFloat2String
 * Description  : turn float to string
 * Parameters   : char* buffer, float value
 * Returns      : string buffer
*******************************************************************************/
char* ICACHE_FLASH_ATTR
DHTFloat2String(char* buffer, float value)
{
  os_sprintf(buffer, "%d.%d", (int)(value),(int)((value - (int)value)*100));
  return buffer;
}
/******************************************************************************
 * FunctionName : peri_dht_read
 * Description  : read dhtxx device
 * Parameters   : DHT_Sensor_Data *output
 * Returns      : true/false
*******************************************************************************/
bool ICACHE_FLASH_ATTR
peri_dht_read(DHT_Sensor_Data *output)
{
	int counter = 0;
	int laststate = 1;
	int i = 0;
	int j = 0;
	int checksum = 0;
	int data[100];
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	uint8_t pin = dht_sensor.pin;

	// Wake up device, 250ms of high
	GPIO_OUTPUT_SET(pin, 1);
	sleepms(250);
	// Hold low for 20ms
	GPIO_OUTPUT_SET(pin, 0);
	sleepms(20);
	// High for 40ns
	GPIO_OUTPUT_SET(pin, 1);
	os_delay_us(40);
	// Set DHT_PIN pin as an input
	GPIO_DIS_OUTPUT(pin);

	// wait for pin to drop?
	while (GPIO_INPUT_GET(pin) == 1 && i < DHT_MAXCOUNT)
	{
		os_delay_us(1);
		i++;
	}

	if(i == DHT_MAXCOUNT)
	{
		PRINTF("DHT: Failed to get reading from GPIO%d, dying\r\n", pin);
	    return false;
	}

	// read data
	for (i = 0; i < DHT_MAXTIMINGS; i++)
	{
		// Count high time (in approx us)
		counter = 0;
		while (GPIO_INPUT_GET(pin) == laststate)
		{
			counter++;
			os_delay_us(1);
			if (counter == 1000)
				break;
		}
		laststate = GPIO_INPUT_GET(pin);
		if (counter == 1000)
			break;
		// store data after 3 reads
		if ((i>3) && (i%2 == 0))
		{
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (counter > DHT_BREAKTIME)
				data[j/8] |= 1;
			j++;
		}
	}

	if (j >= 39)
	{
		checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
	    PRINTF("DHT%s: %02x %02x %02x %02x [%02x] CS: %02x (GPIO%d)\r\n",
	              dht_sensor.type==DHT11?"11":"22",
	              data[0], data[1], data[2], data[3], data[4], checksum, pin);
		if (data[4] == checksum)
		{
			// checksum is valid
			output->temperature = scale_temperature(dht_sensor.type, data);
			output->humidity = scale_humidity(dht_sensor.type, data);
			//PRINTF("DHT: Temperature =  %d *C, Humidity = %d %%\r\n", (int)(reading.temperature * 100), (int)(reading.humidity * 100));
			PRINTF("DHT: Temperature*100 =  %d *C, Humidity*100 = %d %% (GPIO%d)\n",
		          (int) (output->temperature * 100), (int) (output->humidity * 100), pin);
		}
		else
		{
			//PRINTF("Checksum was incorrect after %d bits. Expected %d but got %d\r\n", j, data[4], checksum);
			PRINTF("DHT: Checksum was incorrect after %d bits. Expected %d but got %d (GPIO%d)\r\n",
		                j, data[4], checksum, pin);
		    return false;
		}
	}
	else
	{
		//PRINTF("Got too few bits: %d should be at least 40\r\n", j);
	    PRINTF("DHT: Got too few bits: %d should be at least 40 (GPIO%d)\r\n", j, pin);
	    return false;
	}
	return true;
}

/******************************************************************************
 * FunctionName : peri_dht_init
 * Description  : dhtxx device initialize
 * Parameters   : DHT_Sensor
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
peri_dht_init(DHT_Sensor* sensor)
{
	uint8 pin;
	PRINTF("sensor type:%d", sensor->type);
	PRINTF("sensor pin:%d", sensor->pin);
	dht_sensor.pin = sensor->pin;
	pin = dht_sensor.pin;
	dht_sensor.type = DHT11;
	//set gpio2 as gpio pin
	PIN_FUNC_SELECT(tisan_get_gpio_name(pin), tisan_get_gpio_general_func(pin));
	//disable pulldown
	PIN_PULLDWN_DIS(tisan_get_gpio_name(pin));
	//enable pull up R
	PIN_PULLUP_EN(tisan_get_gpio_name(pin));
    // Configure the GPIO with internal pull-up
	GPIO_DIS_OUTPUT(pin);
	PRINTF("DHT: Setup for type %s connected to GPIO%d\n", dht_sensor.type==DHT11?"DHT11":"DHT22", dht_sensor.pin);
}
