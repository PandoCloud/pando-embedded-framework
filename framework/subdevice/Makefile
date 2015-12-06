SUB_DEV_OBJS=pando_object.o pando_subdevice.o
OBJS = $(SUB_DEV_OBJS)

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)