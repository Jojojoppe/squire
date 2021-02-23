.SILENT:
.PHONY: all x86_generic x86_ide generic_vga

all: x86_generic x86_ide generic_vga

x86_generic:
	cd x86_generic && ${MAKE} ${MFLAGS} all
x86_ide:
	cd x86_ide && ${MAKE} ${MFLAGS} all
generic_vga:
	cd generic_vga && ${MAKE} ${MFLAGS} all

clean:
	cd x86_generic && ${MAKE} ${MFLAGS} clean
	cd x86_ide && ${MAKE} ${MFLAGS} clean
	cd generic_vga && ${MAKE} ${MFLAGS} clean
	
