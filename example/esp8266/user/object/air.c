#include "../../pando/framework/subdevice/pando_object.h"
#include "c_types.h"
#include "user_interface.h"
#include "air.h"
#include "../../peripheral/peri_iaq.h"
// add your own includes below

#define AIR_OBJECT_NO 1
struct air {
	uint16 quality;
};
void ICACHE_FLASH_ATTR
air_init() {
	// TODO: add your object init code here.
	  peri_iaq_init();

}
void ICACHE_FLASH_ATTR
air_set(struct air* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.


}
void ICACHE_FLASH_ATTR
air_get(struct air* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	value->quality=peri_iaq_read();
	//PRINTF("Air Quality: %d\n",value->quality);

}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct air* ICACHE_FLASH_ATTR
create_air() {
	struct air* air = (struct air*)os_malloc(sizeof(air));
	return air;
}
void ICACHE_FLASH_ATTR
delete_air(struct air* air) {
	if(air){
		os_free(air);
	}
}
void ICACHE_FLASH_ATTR
air_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct air* air = create_air();
	air_get(air);

	if (add_next_uint16(params, air->quality)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_air(air);
}
void ICACHE_FLASH_ATTR
air_object_unpack(PARAMS* params) {
	struct air* air = create_air();

	air->quality = get_next_uint16(params);

	air_set(air);
	delete_air(air);
}
void ICACHE_FLASH_ATTR
air_object_init() {
	air_init();
	pando_object air_object = {
		1,
		air_object_pack,
		air_object_unpack,
	};
	register_pando_object(air_object);
}
