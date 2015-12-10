VPATH = mqtt
GATEWAY_OBJS = pando_channel.o pando_cloud_access.o pando_device_login.o pando_device_register.o pando_gateway.o pando_storage_interface.o pando_zero_device.o 
MQTT_OBJS = mqtt.o mqtt_msg.o proto.o queue.o ringbuf.o utils.o
OBJS = $(GATEWAY_OBJS) $(MQTT_OBJS)

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -g -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)