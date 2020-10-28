include ../../makefiles/env.mk

.PHONY: builddir clean crtfiles libc_arch

OUTPUTDIR		:= ../build/$(ARCH)
CFILES			:= $(shell find pdclib -type f -name "*.c")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I ../pdclib/internals -I ../pdclib/include -I pdclib/include -I pdclib/internals -D_PDCLIB_EXTENSIONS

libc_arch: $(OUTPUTDIR)/libc_arch.o

builddir:
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/_PDCLIB
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/stdio
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/stdlib
	-mkdir -p $(OUTPUTDIR)/pdclib/functions/threads

$(OUTPUTDIR)/libc_arch.o: builddir $(COBJ)
	echo + Create libc_arch.o
	echo 'LD ' libc_arch.o
	$(CC) -r -o $@ $(COBJ)

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) -g -MD -o $@ -c $<

crtfiles: $(OUTPUTDIR)/crt1.o $(OUTPUTDIR)/crti.o $(OUTPUTDIR)/crtn.o

$(OUTPUTDIR)/crt1.o: crt1.c
	echo 'CC ' crt1.c
	$(CC) -o $@ -c crt1.c

$(OUTPUTDIR)/crti.o: crti.S
	echo 'CC ' crti.S
	$(CC) -o $@ -c crti.S

$(OUTPUTDIR)/crtn.o: crtn.S
	echo 'CC ' crtn.S
	$(CC) -o $@ -c crtn.S

clean:
	-rm $(OUTPUTDIR)/crt*
	-rm $(OUTPUTDIR)/libc_arch.o
	-rm $(COBJ) $(DFILES)