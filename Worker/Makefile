export BUILD_DIR := $(shell pwd)/Build

export PWD := $(shell pwd)

export SOURCE_DIR := ./src
export INCLUDE_DIR := ./include

export LIBRARIES := -pthread

REGISTRATION_INCLUDE_DIR := $(PWD)/registration/include
UTILS_INCLUDE_DIR := $(PWD)/utils/include
export INCLUDE := -I$(REGISTRATION_INCLUDE_DIR) -I$(UTILS_INCLUDE_DIR)


DIRS := $(filter-out Build/., $(wildcard */.))
DIRS := $(filter-out main/., $(DIRS))
DIRS := $(filter-out tests/., $(DIRS))

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
