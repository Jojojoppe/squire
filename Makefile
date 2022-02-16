.SILENT:

all:
	tup

graph:
	tup scan
	tup graph . | unflatten -c 4 | dot -Tpng > graph.png