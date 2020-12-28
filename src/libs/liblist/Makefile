NAME := liblist
OBJ := liblist.o

LOCAL_SOURCE_DIR := $(CURDIR)/src
LOCAL_INCLUDE_DIR := $(CURDIR)/include

OBJ := $(addprefix $(OBJ_DIR)/$(NAME)/, $(OBJ))

.PHONY: all
all: check_local_obj_dir check_local_lib_dir check_include_dir $(NAME).a copy_public_header

check_local_obj_dir:
	@if [ ! -d "$(OBJ_DIR)/$(NAME)" ]; then mkdir $(OBJ_DIR)/$(NAME); fi

check_local_lib_dir:
	@if [ ! -d "$(LIB_DIR)/$(NAME)" ]; then mkdir $(LIB_DIR)/$(NAME); fi

check_include_dir:
	@if [ ! -d "$(LIB_DIR)/$(NAME)/include" ]; then mkdir $(LIB_DIR)/$(NAME)/include; fi

$(NAME).a: $(OBJ)
	 ar rcs $(LIB_DIR)/$(NAME)/$(NAME).a $(OBJ)

$(OBJ_DIR)/$(NAME)/%.o: $(LOCAL_SOURCE_DIR)/%.c $(LOCAL_INCLUDE_DIR)/%.h $(COMMON_HEADERS)
	$(CC) -g -c -o $@ $< \
					-I$(LOCAL_INCLUDE_DIR)

copy_public_header:
	cp $(LOCAL_INCLUDE_DIR)/$(NAME).h $(LIB_DIR)/$(NAME)/include/$(NAME).h