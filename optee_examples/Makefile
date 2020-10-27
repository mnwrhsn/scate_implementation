export V ?= 0

OUTPUT_DIR := $(CURDIR)/out

EXAMPLE_LIST := $(subst /,,$(dir $(wildcard */Makefile)))

.PHONY: all
all: examples prepare-for-rootfs

.PHONY: clean
clean: examples-clean prepare-for-rootfs-clean

examples:
	@for example in $(EXAMPLE_LIST); do \
		$(MAKE) -C $$example CROSS_COMPILE="$(HOST_CROSS_COMPILE)" || exit -1; \
	done

examples-clean:
	@for example in $(EXAMPLE_LIST); do \
		$(MAKE) -C $$example clean || exit -1; \
	done

prepare-for-rootfs: examples
	@echo "Copying example CA and TA binaries to $(OUTPUT_DIR)..."
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)/ta
	@mkdir -p $(OUTPUT_DIR)/ca
	@for example in $(EXAMPLE_LIST); do \
		if [ -e $$example/host/optee_example_$$example ]; then \
			cp -p $$example/host/optee_example_$$example $(OUTPUT_DIR)/ca/; \
		fi; \
		cp -pr $$example/ta/*.ta $(OUTPUT_DIR)/ta/; \
	done

# mhasan: add our TA

# rover platform
	@cp $(CURDIR)/plat_rover/host/plat_rover $(OUTPUT_DIR)/ca/
	@cp $(CURDIR)/plat_rover/ta/*.ta $(OUTPUT_DIR)/ta/

# robot arm platform
	@cp $(CURDIR)/plat_robot_arm/host/plat_robot_arm $(OUTPUT_DIR)/ca/
	@cp $(CURDIR)/plat_robot_arm/ta/*.ta $(OUTPUT_DIR)/ta/

# syringe pump platform
	@cp $(CURDIR)/plat_syringe_pump/host/plat_syringe_pump $(OUTPUT_DIR)/ca/
	@cp $(CURDIR)/plat_syringe_pump/ta/*.ta $(OUTPUT_DIR)/ta/

# flight controller platform
	@cp $(CURDIR)/plat_flight_controller/host/plat_flight_controller $(OUTPUT_DIR)/ca/
	@cp $(CURDIR)/plat_flight_controller/ta/*.ta $(OUTPUT_DIR)/ta/

# this is for SMC timing experiments
	@cp $(CURDIR)/exp_smc_timing/host/exp_smc_timing $(OUTPUT_DIR)/ca/
	@cp $(CURDIR)/exp_smc_timing/ta/*.ta $(OUTPUT_DIR)/ta/

prepare-for-rootfs-clean:
	@rm -rf $(OUTPUT_DIR)/ta
	@rm -rf $(OUTPUT_DIR)/ca
	@rmdir --ignore-fail-on-non-empty $(OUTPUT_DIR) || test ! -e $(OUTPUT_DIR)
