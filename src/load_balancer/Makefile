NAME := load_balancer
OBJ := clientListener.o heap.o secretary.o utils.o workerListener.o

LOCAL_SOURCE_DIR := $(CURDIR)/src
LOCAL_INCLUDE_DIR := $(CURDIR)/include

LIBS := -llist

OBJ := $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ))

LIBS_INCLUDE_ARG := $(addprefix -I, $(LIBS_INCLUDE_PATH))
LIBS_LINK_ARG := $(addprefix -L, $(LIBS_LINK_PATH))

.PHONY: all
all: check_local_obj_dir $(NAME)

check_local_obj_dir:
	@if [ ! -d "$(OBJ_DIR)/$(NAME)" ]; then mkdir $(OBJ_DIR)/$(NAME); fi

$(NAME): $(OBJ)
	$(CC) -std=gnu99 -g -o $(BIN_DIR)/$(NAME) $(LOCAL_SOURCE_DIR)/main.c $(OBJ) -I$(COMMON_HEADERS_DIR) $(LIBS_INCLUDE_ARG) -I$(LOCAL_INCLUDE_DIR) $(LIBS_LINK_ARG) $(LIBS) $(COMMON_LIBRARIES)

$(OBJ_DIR)/$(NAME)/%.o: $(LOCAL_SOURCE_DIR)/%.c $(LOCAL_INCLUDE_DIR)/%.h $(COMMON_HEADERS)
	$(CC) -std=gnu99 -g -c -o $@ $< -I$(COMMON_HEADERS_DIR) $(LIBS_INCLUDE_ARG) -I$(LOCAL_INCLUDE_DIR) $(LIBS_LINK_ARG) $(LIBS) $(COMMON_LIBRARIES)
