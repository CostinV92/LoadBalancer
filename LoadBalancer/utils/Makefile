OBJ := log.o heap.o

ifdef OBJ

OBJ := $(addprefix $(BUILD_DIR)/, $(OBJ))

all: $(OBJ)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -std=c99 -c -o $@ $< $(INCLUDE) $(LIBRARIES)

endif
.PHONY: all
all: