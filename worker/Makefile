NAME := worker
BUILD_DIR := $(shell pwd)/build

PWD := $(shell pwd)

SOURCE_DIR := ./src
INCLUDE_DIR := ./include
INCLUDE := -I$(INCLUDE_DIR)

LIBRARIES := -pthread

OBJ := registration.o utils.o work.o
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
