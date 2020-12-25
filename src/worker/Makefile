NAME := worker
OBJ := registration.o utils.o work.o

LOCAL_SOURCE_DIR := $(CURDIR)/src
LOCAL_INCLUDE_DIR := $(CURDIR)/include

OBJ := $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ))

.PHONY: all
all: check_local_obj_dir $(NAME)

check_local_obj_dir:
	@if [ ! -d "$(OBJ_DIR)/$(NAME)" ]; then mkdir $(OBJ_DIR)/$(NAME); fi

$(NAME): $(OBJ)
	$(CC) -std=gnu99 -g -o $(BIN_DIR)/$(NAME) $(LOCAL_SOURCE_DIR)/main.c $(OBJ) -I$(COMMON_HEADERS_DIR) -I$(LOCAL_INCLUDE_DIR) $(COMMON_LIBRARIES)

$(OBJ_DIR)/$(NAME)/%.o: $(LOCAL_SOURCE_DIR)/%.c $(LOCAL_INCLUDE_DIR)/%.h $(COMMON_HEADERS)
	$(CC) -std=gnu99 -g -c -o $@ $< -I$(COMMON_HEADERS_DIR) -I$(LOCAL_INCLUDE_DIR) $(COMMON_LIBRARIES)
