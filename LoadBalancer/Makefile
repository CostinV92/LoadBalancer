export NAME := LoadBalancer
export BUILD_DIR := $(shell pwd)/Build

export PWD := $(shell pwd)

export SOURCE_DIR := ./src
export INCLUDE_DIR := ./include
export INCLUDE := -I$(INCLUDE_DIR)

export LIBRARIES := -pthread

OBJ := clientListener.o heap.o secretary.o utils.o workerListener.o
COMMON := messages.h

ifdef OBJ

OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ))

all: BUILD_DIR main

BUILD_DIR:
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/$(COMMON)
	$(CC) -std=gnu99 -c -o $@ $< $(INCLUDE) $(LIBRARIES)

main: $(OBJ)
	$(CC) -std=gnu99 -o $(BUILD_DIR)/$(NAME) $(SOURCE_DIR)/main.c $(OBJ) $(INCLUDE) $(LIBRARIES)

endif

clean: 
	rm -rf $(BUILD_DIR)

.PHONY: all $(DIRS) clean BUILD_DIR