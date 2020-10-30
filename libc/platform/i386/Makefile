include ../../../makefiles/env.mk

.PHONY: builddir clean crtfiles

OUTPUTDIR		:= ../../build/$(ARCH)

CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I $(PREFIX)/usr/include -I $(PREFIX)/include

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
