include ../../../makefiles/env.mk

BUILDDIR			:= ../../build
INSTALLDIR			:= $(PREFIX)/lib

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:=
CC_FLAGS			:= -nostdlib
CC_INCLUDES			:=

.PHONY: all clean install uninstall crtfiles

crtfiles: $(BUILDDIR)/crt1.o $(BUILDDIR)/crti.o $(BUILDDIR)/crtn.o

clean:
	echo CLEAN FILES FOR CRTFILES
	-rm $(BUILDDIR)/{crt1.o,crti.o,crtn.o}

install: $(INSTALLDIR)/crt1.o $(INSTALLDIR)/crti.o $(INSTALLDIR)/crtn.o

uninstall:
	echo UNINSTALL FILES FOR CRTFILES
	-rm $(INSTALLDIR)/{crt1.o,crti.o,crtn.o}

$(INSTALLDIR)/%.o: $(BUILDDIR)/%.o
	echo 'CP ' $< $@
	-mkdir -p $(shell dirname $@)
	cp -RT $< $@

$(BUILDDIR)/crt1.o: crt1.c
	echo 'CC ' $@
	$(CC) -o $@ -c $<

$(BUILDDIR)/crti.o: crti.S
	echo 'AS ' $@
	$(AS) -o $@ -c crti.S

$(BUILDDIR)/crtn.o: crtn.S
	echo 'AS ' $@
	$(AS) -o $@ -c crtn.S
