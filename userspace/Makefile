.SILENT:
.PHONY: all drivers testbin

all: drivers testbin

drivers:
	cd drivers && ${MAKE} ${MFLAGS} all

testbin:
	cd testbin && ${MAKE} ${MFLAGS} all

clean:
	cd drivers && ${MAKE} ${MFLAGS} clean
	
