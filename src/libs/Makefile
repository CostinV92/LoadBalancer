MOD_DIR = liblist
MOD_DIR := $(addprefix $(CURDIR)/, $(MOD_DIR))

.PHONY : all $(MOD_DIR)
all: check_lib_dir $(MOD_DIR)

check_lib_dir:
	@if [ ! -d "$(LIB_DIR)" ]; then mkdir $(LIB_DIR); fi

$(MOD_DIR):
	@$(MAKE) -C $@
	@echo

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)
