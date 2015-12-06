export CC = gcc
TARGET = linux_test
export ROOT_DIR = $(shell pwd)/../
export BUILD_DIR = $(ROOT_DIR)/build
LINUX_EXAMPLE_DIR = $(ROOT_DIR)/example/linux/
export FRAMEWORK_DIR = $(ROOT_DIR)/framework/
export CFLAGS = -I$(FRAMEWORK_DIR)/ -I$(FRAMEWORK_DIR)/platform/include -I$(FRAMEWORK_DIR)/gateway/mqtt/include -I$(ROOT_DIR)

all:
	cd $(LINUX_EXAMPLE_DIR) && $(MAKE)
	cd $(FRAMEWORK_DIR) && $(MAKE)
	