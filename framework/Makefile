FRAMEWORK_OBJS=pando_framework.o
OBJS=$(FRAMEWORK_OBJS) 

all:$(OBJS) 
	cd $(FRAMEWORK_DIR)/platform && $(MAKE)		
	cd $(FRAMEWORK_DIR)/protocol && $(MAKE)
	cd $(FRAMEWORK_DIR)/lib && $(MAKE)
	cd $(FRAMEWORK_DIR)/subdevice && $(MAKE)
	cd $(FRAMEWORK_DIR)/gateway && $(MAKE)
	
	#$(CC) -o $(TARGET) $(BUIL_DIR/*.o) $(CFLAGS)
	
$(OBJS):%.o:%.c
	$(CC) -g -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)