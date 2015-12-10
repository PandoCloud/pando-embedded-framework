VPATH = src/linux
CFLAGS += -I./include
PLATFORM_OBJS = linux_http_impl.o linux_tcp_impl.o linux_timer_impl.o linux_flash_impl.o
OBJS = $(PLATFORM_OBJS)

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -g -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)