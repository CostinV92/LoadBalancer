MODULE_NAME := listener
SOURCES := listener.c

ifdef SOURCES
$(BUILD_DIR)/$(MODULE_NAME).o: $(SOURCE_DIR)/$(SOURCES) $(INCLUDE_DIR)/$(SOURCES:.c=.h)	
	$(CC) -std=c99 -c -o $@ $(SOURCE_DIR)/$(SOURCES) $(INCLUDE) $(LIBRARIES)
endif


