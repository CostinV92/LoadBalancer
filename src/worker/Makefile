NAME := worker
OBJ := registration.o work.o

LOCAL_SOURCE_DIR := $(CURDIR)/src
LOCAL_INCLUDE_DIR := $(CURDIR)/include

LIBS := -lutils

OBJ := $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ))

.PHONY: all
all: check_local_obj_dir $(NAME)

check_local_obj_dir:
	@if [ ! -d "$(OBJ_DIR)/$(NAME)" ]; then mkdir $(OBJ_DIR)/$(NAME); fi

$(NAME): $(OBJ)
	$(CC) -g -o $(BIN_DIR)/$(NAME) $(LOCAL_SOURCE_DIR)/main.c $(OBJ) \
					-I$(COMMON_HEADERS_DIR) \
					-I$(LOCAL_INCLUDE_DIR) \
					$(LIBS_INCLUDE_ARG) \
					$(LIBS_LINK_ARG) \
					$(LIBS) \
					$(COMMON_LIBRARIES)

$(OBJ_DIR)/$(NAME)/%.o: $(LOCAL_SOURCE_DIR)/%.c $(LOCAL_INCLUDE_DIR)/%.h $(COMMON_HEADERS)
	$(CC) -g -c -o $@ $< \
					-I$(COMMON_HEADERS_DIR) \
					-I$(LOCAL_INCLUDE_DIR) \
					$(LIBS_INCLUDE_ARG) \
					$(LIBS_LINK_ARG) \
					$(LIBS) \
					$(COMMON_LIBRARIES)
