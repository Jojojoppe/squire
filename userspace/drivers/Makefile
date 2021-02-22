.SILENT:
.PHONY: all x86_generic generic_ide generic_vga

all: x86_generic generic_ide generic_vga

x86_generic:
	cd x86_generic && ${MAKE} ${MFLAGS} all
generic_ide:
	cd generic_ide && ${MAKE} ${MFLAGS} all
generic_vga:
	cd generic_vga && ${MAKE} ${MFLAGS} all

clean:
	cd x86_generic && ${MAKE} ${MFLAGS} clean
	cd generic_ide && ${MAKE} ${MFLAGS} clean
	cd generic_vga && ${MAKE} ${MFLAGS} clean
	
