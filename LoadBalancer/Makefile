export BUILD_DIR := $(shell pwd)/Build

export PWD := $(shell pwd)

export SOURCE_DIR := ./src
export INCLUDE_DIR := ./include

LISTENER_INCLUDE_DIR := $(PWD)/listener/include
SECRETARY_INCLUDE_DIR := $(PWD)/secretary/include
WORKERLISTENER_INCLUDE_DIR := $(PWD)/workerListener/include
export INCLUDE := -I$(LISTENER_INCLUDE_DIR) -I$(SECRETARY_INCLUDE_DIR) -I$(WORKERLISTENER_INCLUDE_DIR)


DIRS := $(filter-out Build/., $(wildcard */.))
DIRS := $(filter-out main/., $(DIRS))

all: BUILD_DIR $(DIRS) main/.
	
BUILD_DIR:
	if [ ! -d "$(BUILD_DIR)" ]; then mkdir $(BUILD_DIR); fi

$(DIRS):
	$(MAKE) -C $@

main/.: $(DIRS)
	$(MAKE) -C $@

clean: 
	rm -rf $(BUILD_DIR)

.PHONY: all $(DIRS) clean BUILD_DIR
