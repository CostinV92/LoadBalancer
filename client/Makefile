NAME := client
BUILD_DIR := $(shell pwd)/build
VAGRANT_DIR := $(shell pwd)/vagrant/ansible/files
TEST_BED_DIR := /home/victor/Desktop/LICENTA/TestBed/x

PWD := $(shell pwd)

SOURCE_DIR := ./src
INCLUDE_DIR := ./include
INCLUDE := -I$(INCLUDE_DIR)

LIBRARIES := -pthread

OBJ := utils.o
COMMON := messages.h

ifdef OBJ

OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ))

all: BUILD_DIR main1 main2 cp

BUILD_DIR:
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/$(COMMON)
	$(CC) -std=gnu99 -c -o $@ $< $(INCLUDE) $(LIBRARIES)

main1: $(OBJ)
	$(CC) -std=gnu99 -o $(BUILD_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

main2: $(OBJ)
	$(CC) -std=gnu99 -o $(VAGRANT_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

cp:
	cp $(BUILD_DIR)/$(NAME) $(TEST_BED_DIR)/$(NAME)

endif

clean: 
	rm -rf $(BUILD_DIR)

.PHONY: all $(DIRS) clean BUILD_DIR
