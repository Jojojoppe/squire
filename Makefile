.SILENT:

.PHONY: all
all:
	tup

.PHONY: graph
graph:
	tup scan
	tup graph . | unflatten -c 4 | dot -Tpng > graph.png

.PHONY: menuconfig
menuconfig:
	KCONFIG_CONFIG=build-default/tup.config scripts/menuconfig.py

.PHONY: run_arm
run_arm:
	echo Starting QEMU arm
	echo \'ctrl-a x\' to stop VM
	qemu-system-arm -machine xilinx-zynq-a9 -kernel build-default/kernel/kernel -serial /dev/null -serial mon:stdio -nographic -m 256M

.PHONY: test
test:
	echo Run tests
	build-default/kernel/test/test