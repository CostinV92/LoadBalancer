NAME := load_balancer
OBJ := connections.o client_listener.o worker_listener.o heap.o

LOCAL_SOURCE_DIR := $(CURDIR)/src
LOCAL_INCLUDE_DIR := $(CURDIR)/include

LIBS := -llist -lutils

OBJ := $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ))

.PHONY: all
all: check_local_obj_dir $(NAME)

check_local_obj_dir:
	@if [ ! -d "$(OBJ_DIR)/$(NAME)" ]; then mkdir $(OBJ_DIR)/$(NAME); fi

$(NAME): $(OBJ)
	$(CC) -g -o $(BIN_DIR)/$(NAME) $(LOCAL_SOURCE_DIR)/main.c $(OBJ) \
					-I$(LOCAL_INCLUDE_DIR) \
					-I$(COMMON_HEADERS_DIR) \
					$(LIBS_INCLUDE_ARG) \
					$(LIBS_LINK_ARG) \
					$(LIBS) \
					$(COMMON_LIBRARIES)

$(OBJ_DIR)/$(NAME)/%.o: $(LOCAL_SOURCE_DIR)/%.c $(LOCAL_INCLUDE_DIR)/%.h $(COMMON_HEADERS)
	$(CC) -g -c -o $@ $< \
					-I$(LOCAL_INCLUDE_DIR) \
					-I$(COMMON_HEADERS_DIR) \
					$(LIBS_INCLUDE_ARG) \
					$(LIBS_LINK_ARG) \
					$(LIBS) \
					$(COMMON_LIBRARIES)
