#include "../../peripheral/peri_dht1122.h"
#include "../../pando/framework/subdevice/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "humiture.h"
// add your own includes below
#define HUMITURE_OBJECT_NO 1

struct humiture {
	float32 percent;
};
void ICACHE_FLASH_ATTR
humiture_init() {
	// TODO: add your object init code here.
	DHT_Sensor dht_sensor;
	dht_sensor.pin = 12;
	dht_sensor.type = DHT11;
	peri_dht_init(&dht_sensor);

}
void ICACHE_FLASH_ATTR
humiture_set(struct humiture* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.


}
void ICACHE_FLASH_ATTR
humiture_get(struct humiture* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	DHT_Sensor_Data output;
    peri_dht_read(&output);
    value->percent = output.humidity;

}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct humiture* ICACHE_FLASH_ATTR
create_humiture() {
	struct humiture* humiture = (struct humiture*)os_malloc(sizeof(humiture));
	return humiture;
}
void ICACHE_FLASH_ATTR
delete_humiture(struct humiture* humiture) {
	if(humiture){
		os_free(humiture);
	}
}
void ICACHE_FLASH_ATTR
humiture_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct humiture* humiture = create_humiture();
	humiture_get(humiture);

	if (add_next_float32(params, humiture->percent)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_humiture(humiture);
}
void ICACHE_FLASH_ATTR
humiture_object_unpack(PARAMS* params) {
	struct humiture* humiture = create_humiture();

	humiture->percent = get_next_float32(params);

	humiture_set(humiture);
	delete_humiture(humiture);
}
void ICACHE_FLASH_ATTR
humiture_object_init() {
	humiture_init();
	pando_object humiture_object = {
		1,
		humiture_object_pack,
		humiture_object_unpack,
	};
	register_pando_object(humiture_object);
}
