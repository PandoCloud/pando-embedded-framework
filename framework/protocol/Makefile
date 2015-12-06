PROTOCOL_OBJS=common_functions.o pando_endian.o pando_machine.o pando_protocol.o sub_device_protocol.o
OBJS = $(PROTOCOL_OBJS)

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)