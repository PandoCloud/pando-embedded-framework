export CC = gcc
TARGET = linux_test
export ROOT_DIR = $(shell pwd)/../
export BUILD_DIR = $(ROOT_DIR)/build/target
LINUX_EXAMPLE_DIR = $(ROOT_DIR)/example/linux/
export FRAMEWORK_DIR = $(ROOT_DIR)/framework/
export CFLAGS = -I$(FRAMEWORK_DIR)/ -I$(FRAMEWORK_DIR)/platform/include -I$(FRAMEWORK_DIR)/gateway/mqtt/include -I$(ROOT_DIR) -lssl -lcrypto
OBJS = 

$(TARGET):
	cd $(FRAMEWORK_DIR) && $(MAKE)
	cd $(LINUX_EXAMPLE_DIR) && $(MAKE)
	$(CC) -o $(TARGET) $(BUILD_DIR)/*.o $(CFLAGS)
clean:
	-$(RM) $(BUILD_DIR)/*.o
	-$(RM) $(BUILD_DIR)/$(TARGET)
	