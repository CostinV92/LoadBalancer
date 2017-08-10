MODULE_NAME := LoadBalancer
SOURCES := main.c
OBJ := $(wildcard $(BUILD_DIR)/*.o)

$(BUILD_DIR)/$(MODULE_NAME): $(SOURCES) $(OBJ)
	$(CC) -o $@ $^ $(INCLUDE) 

