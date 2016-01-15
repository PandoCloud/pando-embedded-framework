
#include "pando_object.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "sub_device_protocol.h"

#include "plug.h"


struct plug {
	uint8_t off_on;
};
void plug_init() {
	// TODO: add your object init code here.
	//peri_jdq_init(12);

}
void plug_set(struct plug* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
	//peri_jdq_set(value->off_on);

}
void plug_get(struct plug* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	//value->off_on=peri_jdq_get();
}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct plug* create_plug() {
	struct plug* plug = (struct plug*)malloc(sizeof(plug));
	return plug;
}
void delete_plug(struct plug* plug) {
	if(plug){
		free(plug);
	}
}
void plug_object_pack(PARAMS * params) {
	if(NULL == params){
		printf("Create first tlv param failed.\n");
		return;
	}

	struct plug* plug = create_plug();
	plug_get(plug);

	if (add_next_uint8(params, plug->off_on)){
		printf("Add next param failed.\n");
		return;
	}

	delete_plug(plug);
}
void plug_object_unpack(PARAMS* params) {
	struct plug* plug = create_plug();

	plug->off_on = get_next_uint8(params);

	plug_set(plug);
	delete_plug(plug);
}
void plug_object_init() {
	plug_init();
    
	pando_object plug_object = {
		1,
		plug_object_pack,
		plug_object_unpack,
	};
	register_pando_object(plug_object);
}

