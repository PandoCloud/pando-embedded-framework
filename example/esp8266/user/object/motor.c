#include "../../pando/framework/subdevice/pando_object.h"
#include "../../peripheral/peri_motor.h"
#include "c_types.h"
#include "user_interface.h"
#include "motor.h"
// add your own includes below


struct motor {
	int8 speed;
};
void ICACHE_FLASH_ATTR
motor_init() {
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
motor_set(struct motor* value) {
	// TODO: implement object set function here.
	// the set function read value and operate the hardware.
	struct PWM_APP_PARAM motor_param;
	motor_param.pwm_freq=25000;
	if((value->speed)>=0)   //forward
	{
		motor_param.pwm_duty[0]=value->speed;
		motor_param.pwm_duty[1]=0;

	}
	else
	{
		motor_param.pwm_duty[0]=0;
		motor_param.pwm_duty[1]=0-(value->speed);

	}

	peri_motor_set(motor_param);

}
void ICACHE_FLASH_ATTR
motor_get(struct motor* value) {
	// TODO: implement object get  function here
	// get function retrieve hardware status and assign it to value.
	struct PWM_APP_PARAM motor_param;
	motor_param=peri_motor_get();
	if(motor_param.pwm_duty[0]==0)
	{
		value->speed=0-(motor_param.pwm_duty[1]);
	}
	if(motor_param.pwm_duty[1]==0)
	{
		value->speed=(motor_param.pwm_duty[0]);
	}

}
/*
	auto generated code below!!
	DO NOT edit unless you know how it works.
*/
struct motor* ICACHE_FLASH_ATTR
create_motor() {
	struct motor* motor = (struct motor*)os_malloc(sizeof(motor));
	return motor;
}
void ICACHE_FLASH_ATTR
delete_motor(struct motor* motor) {
	if(motor){
		os_free(motor);
	}
}
void ICACHE_FLASH_ATTR
motor_object_pack(PARAMS * params) {
	if(NULL == params){
		PRINTF("Create first tlv param failed.\n");
		return;
	}

	struct motor* motor = create_motor();
	motor_get(motor);

	if (add_next_int8(params, motor->speed)){
		PRINTF("Add next param failed.\n");
		return;
	}

	delete_motor(motor);
}
void ICACHE_FLASH_ATTR
motor_object_unpack(PARAMS* params) {
	struct motor* motor = create_motor();

	motor->speed = get_next_int8(params);

	motor_set(motor);
	delete_motor(motor);
}
void ICACHE_FLASH_ATTR
motor_object_init() {
	motor_init();
	pando_object motor_object = {
		1,
		motor_object_pack,
		motor_object_unpack,
	};
	register_pando_object(motor_object);
}
