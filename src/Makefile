MOD_DIR = client worker load_balancer
MOD_DIR := $(addprefix $(CURDIR)/, $(MOD_DIR))

COMMON_HEADERS = messages.h

export COMMON_HEADERS := $(addprefix $(COMMON_HEADERS_DIR)/, $(COMMON_HEADERS))
export COMMON_LIBRARIES := -pthread

LIBS := liblist
export LIBS_INCLUDE_PATH := $(addprefix $(LIB_DIR)/, $(LIBS)/include)
export LIBS_LINK_PATH := $(addprefix $(LIB_DIR)/, $(LIBS))

.PHONY : all libs $(MOD_DIR)
all: check_obj_dir check_bin_dir libs $(MOD_DIR)

check_obj_dir:
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi

check_bin_dir:
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir $(BIN_DIR); fi

libs:
	@$(MAKE) -C $(CURDIR)/libs


$(MOD_DIR):
	@$(MAKE) -C $@
	@echo

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
