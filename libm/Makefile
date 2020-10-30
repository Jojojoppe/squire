include ../makefiles/env.mk

.PHONY: all libm headers builddir clean

# No all target
all:

OUTPUTDIR			:= build

CFILES				:= $(shell find . -type f -name "*.c")
COBJ				:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES				:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I $(PREFIX)/usr/include -I $(PREFIX)/lib/tcc/include -I $(PREFIX)/include -D_IEEE_LIBM
AR				:= $(PREFIX)/bin/$(ARCH)-tcc -ar
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)

libm: headers builddir $(COBJ)
	echo 'AR ' libm
	$(AR) rcsv libm.a $(COBJ)
	cp libm.a $(PREFIX)/usr/lib

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) -g -MD $(CC_WARNING) -o $@ -c $<

headers:
	echo + Install headers
	cp fdlibm.h $(PREFIX)/usr/include

clean:
	echo + Clean libc
	-rm -r build
	-rm libm.a

-include $(DFILES)
