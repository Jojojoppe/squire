.SILENT:
.PHONY: all x86_generic

all: x86_generic

x86_generic:
	cd x86_generic && ${MAKE} ${MFLAGS} all
	cd ide && ${MAKE} ${MFLAGS} all

clean:
	cd x86_generic && ${MAKE} ${MFLAGS} clean
	cd ide && ${MAKE} ${MFLAGS} clean
	
