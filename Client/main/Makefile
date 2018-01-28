MODULE_NAME := Client
SOURCES := main.c
OBJ := $(wildcard $(BUILD_DIR)/*.o)

$(BUILD_DIR)/$(MODULE_NAME): $(SOURCES) $(OBJ)
	$(CC) -std=gnu99 -o $@ $^ $(INCLUDE) $(LIBRARIES)

