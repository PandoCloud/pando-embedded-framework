VPATH = json
LIB_OBJS = converter.o pando_json.o
JSON_OBJS = jsonparse.o jsontree.o
OBJS = $(LIB_OBJS) $(JSON_OBJS)

all:$(OBJS)

$(OBJS):%.o:%.c
	$(CC) -g -c $< -o $(BUILD_DIR)/$@ $(CFLAGS)