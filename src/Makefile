MOD_DIR = client worker load_balancer
MOD_DIR := $(addprefix $(CURDIR)/, $(MOD_DIR))

COMMON_HEADERS = messages.h

export COMMON_HEADERS := $(addprefix $(COMMON_HEADERS_DIR)/, $(COMMON_HEADERS))
export COMMON_LIBRARIES := -pthread

LIBS := liblist libheap libutils
LIBS_INCLUDE_PATH := $(addprefix $(LIB_DIR)/, $(LIBS))
LIBS_INCLUDE_PATH := $(addsuffix /include, $(LIBS_INCLUDE_PATH))
LIBS_LINK_PATH := $(addprefix $(LIB_DIR)/, $(LIBS))
export LIBS_INCLUDE_ARG := $(addprefix -I, $(LIBS_INCLUDE_PATH))
export LIBS_LINK_ARG := $(addprefix -L, $(LIBS_LINK_PATH))

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
	@echo

clean:
	rm -rf $(OBJ_DIR)
	@echo
	rm -rf $(BIN_DIR)
	@echo
	rm -rf $(LIB_DIR)
