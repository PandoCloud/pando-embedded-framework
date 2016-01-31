#include "../../pando/framework/subdevice/pando_object.h"
#include "../../peripheral/peri_pm25.h"
#include "../../peripheral/driver/uart.h"
#include "c_types.h"
#include "user_interface.h"
#include "pm25.h"

// add your own includes below
#define PM25_OBJECT_NO 1

struct pm25 {
	uint16 quality;
};
void ICACHE_FLASH_ATTR
pm25_init() {
	// TODO: add your object init code here.


}
void ICACHE_FLASH_ATTR
pm25_set(struct pm25* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.


}
void ICACHE_FLASH_ATTR
pm25_get(struct pm25* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	static os_timer_t read_timer;
	 uart_rx_intr_enable(UART0);

	    os_timer_disarm(&read_timer);
		os_timer_setfn(&read_timer, (os_timer_func_t *)peri_pm_25_get,NULL);
		os_timer_arm(&read_timer, 1000, 0);

		value->quality=peri_pm_25_get();
		//PRINTF("PM25_VALUE:%d \n",value->quality);
}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct pm25* ICACHE_FLASH_ATTR
create_pm25() {
	struct pm25* pm25 = (struct pm25*)os_malloc(sizeof(pm25));
	return pm25;
}
void ICACHE_FLASH_ATTR
delete_pm25(struct pm25* pm25) {
	if(pm25){
		os_free(pm25);
	}
}
void ICACHE_FLASH_ATTR
pm25_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct pm25* pm25 = create_pm25();
	pm25_get(pm25);

	if (add_next_uint16(params, pm25->quality)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_pm25(pm25);
}
void ICACHE_FLASH_ATTR
pm25_object_unpack(PARAMS* params) {
	struct pm25* pm25 = create_pm25();

	pm25->quality = get_next_uint16(params);

	pm25_set(pm25);
	delete_pm25(pm25);
}
void ICACHE_FLASH_ATTR
pm25_object_init() {
	pm25_init();
	pando_object pm25_object = {
		1,
		pm25_object_pack,
		pm25_object_unpack,
	};
	register_pando_object(pm25_object);
}
