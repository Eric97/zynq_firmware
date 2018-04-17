SRC_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

define cmake-build
+@$(eval BUILD_DIR = $(SRC_DIR)/build)
+@if [ ! -e $(BUILD_DIR)/CMakeCache.txt ]; then mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake -DCMAKE_TOOLCHAIN_FILE=$(SRC_DIR)/cmake/Toolchain-arm-xilinx-linux-gnueabi.cmake $(SRC_DIR) || (rm -rf $(BUILD_DIR)); fi
+@(cd $(BUILD_DIR) && make)
endef


# targets
firmware:
	$(call cmake-build,$@,$(SRC_DIR))

# Cleanup
.PHONY: clean distclean

clean:
	@rm -rf $(SRC_DIR)/build
