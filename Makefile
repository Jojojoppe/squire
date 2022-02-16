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