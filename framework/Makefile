#CC=gcc
#CFLAGS=-I./ -I./platform/include -I./gateway/mqtt/include
#VPATH=gateway:gateway/mqtt:protocol:subdevice
#GATEWAY_OBJS=pando_channel.o pando_cloud_access.o pando_device_login.o pando_device_register.o pando_gateway.o pando_storage_interface.o pando_zero_device.o 
#MQTT_OBJS=mqtt.o mqtt_msg.o proto.o queue.o ringbuf.o utils.o
#PROTOCOL_OBJS=common_functions.o pando_endian.o pando_machine.o pando_protocol.o sub_device_protocol.o
#SUB_DEV_OBJS=pando_object.o pando_subdevice.o
FRAMEWORK_OBJS=pando_framework.o
OBJS=$(FRAMEWORK_OBJS) 
#$(GATEWAY_OBJS) $(MQTT_OBJS) $(PROTOCOL_OBJS) $(SUB_DEV_OBJS) 
TARGET=framework

all:$(OBJS) 
	cd $(FRAMEWORK_DIR)/gateway && $(MAKE)
	cd $(FRAMEWORK_DIR)/lib && $(MAKE)
	cd $(FRAMEWORK_DIR)/protocol && $(MAKE)
	cd $(FRAMEWORK_DIR)/subdevice && $(MAKE)
	cd $(FRAMEWORK_DIR)/platform && $(MAKE)	
	#$(CC) -o $(TARGET) $(BUIL_DIR/*.o) $(CFLAGS)
	
$(OBJS):%.o:%.c
	$(CC) -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)