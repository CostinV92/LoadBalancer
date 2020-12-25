MOD_DIR = client load_balancer worker
MOD_DIR := $(addprefix $(PROJ_DIR)/, $(MOD_DIR))

COMMON_HEADERS = messages.h
export COMMON_HEADERS := $(addprefix $(COMMON_HEADERS_DIR)/, $(COMMON_HEADERS))
export COMMON_LIBRARIES := -pthread

.PHONY : all $(MOD_DIR)
all: check_obj_dir check_bin_dir $(MOD_DIR)

check_obj_dir:
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi

check_bin_dir:
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir $(BIN_DIR); fi

$(MOD_DIR):
	@$(MAKE) -C $@
	@echo

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)