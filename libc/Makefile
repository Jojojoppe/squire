include ../makefiles/env.mk

.PHONY: all libc headers crtfiles builddir clean

# No all target
all:

OUTPUTDIR			:= build

DIRS				:= functions
CFILES				:= $(shell find $(DIRS) -type f -name "*.c")
COBJ				:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES				:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -D_PDCLIB_EXTENSIONS
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/functions/_PDCLIB
	-mkdir -p $(OUTPUTDIR)/functions/_dlmalloc
	-mkdir -p $(OUTPUTDIR)/functions/ctype
	-mkdir -p $(OUTPUTDIR)/functions/errno
	-mkdir -p $(OUTPUTDIR)/functions/inttypes
	-mkdir -p $(OUTPUTDIR)/functions/locale
	-mkdir -p $(OUTPUTDIR)/functions/signal
	-mkdir -p $(OUTPUTDIR)/functions/stdio
	-mkdir -p $(OUTPUTDIR)/functions/stdlib
	-mkdir -p $(OUTPUTDIR)/functions/string
	-mkdir -p $(OUTPUTDIR)/functions/time
	-mkdir -p $(OUTPUTDIR)/functions/uchar
	-mkdir -p $(OUTPUTDIR)/functions/wchar
	-mkdir -p $(OUTPUTDIR)/functions/wctype
	-mkdir -p $(OUTPUTDIR)/functions/threads
	-mkdir -p $(OUTPUTDIR)/$(ARCH)

libc: headers builddir $(COBJ) crtfiles
	echo 'LD ' libc
	$(CC) -r -static -o libc.a $(COBJ)
	cp libc.a $(PREFIX)/usr/lib/libc.a

crtfiles:
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} crtfiles
	echo + Install crt files
	cp $(OUTPUTDIR)/$(ARCH)/crt* $(PREFIX)/usr/lib

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) -g -MD -I include -I internals -I $(PREFIX)/usr/include $(CC_WARNING) -o $@ -c $<

headers:
	echo + Install headers
	cp -RT include $(PREFIX)/include
	cp -RT internals $(PREFIX)/include

clean:
	echo + Clean libc
	-rm -r build
	-rm libc.a
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} clean

-include $(DFILES)
