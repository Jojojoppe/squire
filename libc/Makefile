include ../makefiles/env.mk

.PHONY: all libc headers crtfiles builddir clean

# No all target
all:

OUTPUTDIR			:= build

FUNCTIONFILES		:= $(shell find pdclib/functions -type f -name "*.c")
FUNCTIONOBJ			:= $(FUNCTIONFILES:%.c=$(OUTPUTDIR)/%.c.o)
FUNCTIOND			:= $(FUNCTIONFILES:%.c=$(OUTPUTDIR)/%.c.d)

INTERNALFILES		:= $(shell find pdclib/internals -type f -name "*.c")
INTERNALOBJ			:= $(INTERNALFILES:%.c=$(OUTPUTDIR)/%.c.o)
INTERNALD			:= $(INTERNALFILES:%.c=$(OUTPUTDIR)/%.c.d)

COBJ				:= $(INTERNALOBJ) $(FUNCTIONOBJ)
DFILES				:= $(INTERNALD) $(FUNCTIOND)

CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I pdclib/internals -I pdclib/include -I $(ARCH)/pdclib/include -I $(ARCH)/pdclib/internals -I $(PREFIX)/usr/include -D_PDCLIB_EXTENSIONS
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/_PDCLIB
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/_dlmalloc
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/ctype
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/errno
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/inttypes
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/locale
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/signal
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/stdio
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/stdlib
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/string
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/time
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/uchar
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/wchar
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/wctype
	-mkdir -p $(OUTPUTDIR)/pdclib/internals
	-mkdir -p $(OUTPUTDIR)/$(ARCH)

$(OUTPUTDIR)/$(ARCH)/libc_arch.o:
	cd $(ARCH) && ${MAKE} ${MFLAGS} libc_arch

libc: headers builddir $(OUTPUTDIR)/$(ARCH)/libc_arch.o $(COBJ) crtfiles
	echo 'LD ' libc
	$(CC) -r -static -o libc.a $(COBJ) $(OUTPUTDIR)/$(ARCH)/libc_arch.o
	cp libc.a $(PREFIX)/usr/lib/libc.a

crtfiles:
	cd $(ARCH) && ${MAKE} ${MFLAGS} crtfiles
	echo + Install crt files
	cp $(OUTPUTDIR)/$(ARCH)/crt* $(PREFIX)/usr/lib

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) -g -MD $(CC_WARNING) -o $@ -c $<

headers:
	echo + Install headers
	cp -RT pdclib/include $(PREFIX)/include
	cp -RT pdclib/internals $(PREFIX)/include
	cp -RT $(ARCH)/pdclib/include $(PREFIX)/include
	cp -RT $(ARCH)/pdclib/internals $(PREFIX)/include

clean:
	echo + Clean libc
	-rm -r build
	-rm libc.a
	cd $(ARCH) && ${MAKE} ${MFLAGS} clean

-include $(DFILES)
