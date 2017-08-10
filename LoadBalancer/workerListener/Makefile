MODULE_NAME := workListener
SOURCES :=

ifdef SOURCES
$(BUILD_DIR)/$(MODULE_NAME).o: $(SOURCE_DIR)/$(SOURCES) $(INCLUDE_DIR)/$(SOURCES:.c=.h)	
	$(CC) -c -o $@ $(SOURCE_DIR)/$(SOURCES) $(INCLUDE)
endif

.PHONY: all
all:
