MODULE_NAME := LoadBalancer
SOURCES := main.c
OBJ := listener.o 

$(BUILD_DIR)/$(MODULE_NAME): $(SOURCES) $(BUILD_DIR)/$(OBJ) 	
	$(CC) -o $@ $^ $(INCLUDE) 

