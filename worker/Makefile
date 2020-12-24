NAME := worker
BUILD_DIR := $(shell pwd)/build
VAGRANT_DIR := $(shell pwd)/vagrant/ansible/files
TEST_BED_DIR := /home/victor/Desktop/LICENTA/test_bed/x

PWD := $(shell pwd)

SOURCE_DIR := ./src
INCLUDE_DIR := ./include
INCLUDE := -I$(INCLUDE_DIR)

LIBRARIES := -pthread

OBJ := registration.o utils.o work.o
COMMON := messages.h

ifdef OBJ

OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ))

all: BUILD_DIR main main2 cp

BUILD_DIR:
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/$(COMMON)
	$(CC) -std=gnu99 -g -c -o $@ $< $(INCLUDE) $(LIBRARIES) -DLOAD_BALANCER_LOCAL_HOST_DEBUG

main: $(OBJ)
	$(CC) -std=gnu99 -g -o $(BUILD_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES) -DLOAD_BALANCER_LOCAL_HOST_DEBUG

main2: $(OBJ)
	$(CC) -std=gnu99 -o $(VAGRANT_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

cp:
	cp $(BUILD_DIR)/$(NAME) $(TEST_BED_DIR)/$(NAME)

endif

clean: 
	rm -rf $(BUILD_DIR)

.PHONY: all $(DIRS) clean BUILD_DIR
