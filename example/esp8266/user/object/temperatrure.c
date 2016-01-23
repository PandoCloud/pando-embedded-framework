#include "../../peripheral/peri_dht1122.h"
#include "../../pando/framework/subdevice/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "temperature.h"
#include "mem.h"
// add your own includes below


struct temperature {
	uint8 centigrade;
};
void ICACHE_FLASH_ATTR
temperature_init() {
	// TODO: add your object init code here.
	DHT_Sensor dht_sensor;
	dht_sensor.pin = 12;
	dht_sensor.type = DHT11;
	peri_dht_init(&dht_sensor);

}
void ICACHE_FLASH_ATTR
temperature_set(struct temperature* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.


}
void ICACHE_FLASH_ATTR
temperature_get(struct temperature* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	DHT_Sensor_Data output;
    peri_dht_read(&output);
    value->centigrade = output.temperature;

}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct temperature* ICACHE_FLASH_ATTR
create_temperature() {
	struct temperature* temperature = (struct temperature*)os_malloc(sizeof(temperature));
	return temperature;
}
void ICACHE_FLASH_ATTR
delete_temperature(struct temperature* temperature) {
	if(temperature){
		os_free(temperature);
	}
}
void ICACHE_FLASH_ATTR
temperature_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct temperature* temperature = create_temperature();
	temperature_get(temperature);

	if (add_next_uint8(params, temperature->centigrade)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_temperature(temperature);
}
void ICACHE_FLASH_ATTR
temperature_object_unpack(PARAMS* params) {
	struct temperature* temperature = create_temperature();

	temperature->centigrade = get_next_uint8(params);

	temperature_set(temperature);
	delete_temperature(temperature);
}
void ICACHE_FLASH_ATTR
temperature_object_init() {
	temperature_init();
	pando_object temperature_object = {
		1,
		temperature_object_pack,
		temperature_object_unpack,
	};
	register_pando_object(temperature_object);
}
