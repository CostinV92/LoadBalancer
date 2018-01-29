export NAME := client
export BUILD_DIR := $(shell pwd)/build
export VAGRANT_DIR := $(shell pwd)/vagrant/ansible/files

export PWD := $(shell pwd)

export SOURCE_DIR := ./src
export INCLUDE_DIR := ./include
export INCLUDE := -I$(INCLUDE_DIR)

export LIBRARIES := -pthread

OBJ := utils.o
COMMON := messages.h

ifdef OBJ

OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ))

all: BUILD_DIR main1 main2

BUILD_DIR:
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/$(COMMON)
	$(CC) -std=gnu99 -c -o $@ $< $(INCLUDE) $(LIBRARIES)

main1: $(OBJ)
	$(CC) -std=gnu99 -o $(BUILD_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

main2: $(OBJ)
	$(CC) -std=gnu99 -o $(VAGRANT_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

endif

clean: 
	rm -rf $(BUILD_DIR)

.PHONY: all $(DIRS) clean BUILD_DIR
