.PHONY: build
all: build

build:
	@$(MAKE) -C $(SRC_DIR)

clean:
	@$(MAKE) clean -C $(SRC_DIR)