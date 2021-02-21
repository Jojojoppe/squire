.SILENT:
.PHONY: all x86_generic generic_ide

all: x86_generic generic_ide

x86_generic:
	cd x86_generic && ${MAKE} ${MFLAGS} all
generic_ide:
	cd generic_ide && ${MAKE} ${MFLAGS} all

clean:
	cd x86_generic && ${MAKE} ${MFLAGS} clean
	cd generic_ide && ${MAKE} ${MFLAGS} clean
	
