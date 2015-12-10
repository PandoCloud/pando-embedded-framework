OBJS = main.o
TARGET = 

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -g -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)	

clean:
	-$(RM) $(BUILD_DIR)/$(OBJS)