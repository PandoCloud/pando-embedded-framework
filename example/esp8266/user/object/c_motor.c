#include "../../pando/framework/subdevice/pando_object.h"
#include "../../peripheral/peri_motor.h"
#include "c_types.h"
#include "user_interface.h"
#include "c_motor.h"
#include "mem.h"
// add your own includes below


struct c_motor {
	int8 content;
};
void ICACHE_FLASH_ATTR
c_motor_init() {
	// TODO: add your object init code here.
	// TODO: add your object init code here.
	struct PWM_APP_PARAM motor_param;
	struct PWM_INIT motor_init;
	motor_param.pwm_freq=25000;
	motor_param.pwm_duty[0]=0;
	motor_param.pwm_duty[1]=0;

	motor_init.io_num=2;
	motor_init.io_id[0]=12;
	motor_init.io_id[1]=13;

	peri_motor_init(motor_param,motor_init);

}
void ICACHE_FLASH_ATTR
c_motor_set(struct c_motor* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
	struct PWM_APP_PARAM motor_param;
	motor_param.pwm_freq=25000;
	if((value->content)>=0)   //forward
	{
		motor_param.pwm_duty[0]=value->content;
		motor_param.pwm_duty[1]=0;

	}
	else
	{
		motor_param.pwm_duty[0]=0;
		motor_param.pwm_duty[1]=0-(value->content);

	}

	peri_motor_set(motor_param);

}
void ICACHE_FLASH_ATTR
c_motor_get(struct c_motor* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	struct PWM_APP_PARAM motor_param;
	motor_param=peri_motor_get();
	if(motor_param.pwm_duty[0]==0)
	{
		value->content=0-(motor_param.pwm_duty[1]);
	}
	if(motor_param.pwm_duty[1]==0)
	{
		value->content=(motor_param.pwm_duty[0]);
	}

}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct c_motor* ICACHE_FLASH_ATTR
create_c_motor() {
	struct c_motor* c_motor = (struct c_motor*)os_malloc(sizeof(c_motor));
	return c_motor;
}
void ICACHE_FLASH_ATTR
delete_c_motor(struct c_motor* c_motor) {
	if(c_motor){
		os_free(c_motor);
	}
}
void ICACHE_FLASH_ATTR
c_motor_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct c_motor* c_motor = create_c_motor();
	c_motor_get(c_motor);

	if (add_next_int8(params, c_motor->content)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_c_motor(c_motor);
}
void ICACHE_FLASH_ATTR
c_motor_object_unpack(PARAMS* params) {
	struct c_motor* c_motor = create_c_motor();

	c_motor->content = get_next_int8(params);

	c_motor_set(c_motor);
	delete_c_motor(c_motor);
}
void ICACHE_FLASH_ATTR
c_motor_object_init() {
	c_motor_init();
	pando_object c_motor_object = {
		2,
		c_motor_object_pack,
		c_motor_object_unpack,
	};
	register_pando_object(c_motor_object);
}
