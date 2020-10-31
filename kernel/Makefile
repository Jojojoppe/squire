include ../makefiles/env.mk

TARGET				:= kernel.bin

BUILDDIR			:= build

SRCDIRS				:= src/general
SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.o)
DEPFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.d)

INFILES				:= $(shell find $(SRCDIRS) -type f -name "*.h.in")
OUTFILES			:= $(INFILES:%.h.in=$(BUILDDIR)/%.h)

INSTALLTARGET		:= ../mnt/boot

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET) $(OUTFILES)
FILESTOUNINSTALL	:=  $(INSTALLTARGET)/$(TARGET)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

# Kernel specific points
SQUIRE_COMMIT  	:= $(shell git rev-parse --short HEAD)
SQUIRE_VMAJOR	:= v0.2-
SQUIRE_VERSION	:= $(SQUIRE_VMAJOR)$(SQUIRE_COMMIT)

# Include arch dependent makefile
# MUST DEFINE CC_ARCH_FLAGS
-include src/$(ARCH)/arch.mk

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:=
CC_FLAGS			:= -nostdlib -nostdinc -DSQUIRE_ARCH=\"$(ARCH)\" -DSQUIRE_VERSION=\"$(SQUIRE_VERSION)\" -O0 -g
CC_INCLUDES			:= -I src -I src/general -I $(BUILDDIR)/src -I ../libsquire/include

LD_FLAGS			:= -nostdinc -nostdlib -static $(CC_ARCH_FLAGS)

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean install uninstall libarch infiles test
all: $(TARGET) install

# PHONY RULES
# -----------

$(TARGET): infiles libarch $(BUILDDIR)/$(TARGET)

clean:
	echo CLEAN FILES FOR $(TARGET)
	-rm -r $(FILESTOCLEAN)
	# Subfolders
	cd src/$(ARCH) && ${MAKE} ${MFLAGS} clean

install: $(INSTALLTARGET)/$(TARGET)

uninstall:
	echo UNINSTALL FILES FOR $(TARGET)
	-rm $(FILESTOUNINSTALL)
	# Subfolders
	cd src/$(ARCH) && ${MAKE} ${MFLAGS} uninstall

# Subfolders
libarch:
	cd src/$(ARCH) && ${MAKE} ${MFLAGS} libarch.o

infiles: $(OUTFILES)

# SPECIFIC BUILD RULES
# --------------------

$(INSTALLTARGET)/$(TARGET): $(BUILDDIR)/$(TARGET)
	echo 'CP ' $(BUILDDIR)/$(TARGET) $(INSTALLTARGET)/$(TARGET)
	cp $(BUILDDIR)/$(TARGET) $(INSTALLTARGET)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(BUILDDIR)/src/$(ARCH)/libarch.o $(OBJFILES)
	echo 'LD ' $@
	$(LD) $(LD_FLAGS) -o $(BUILDDIR)/$(TARGET) $(BUILDDIR)/src/$(ARCH)/libarch.o $(OBJFILES) $(PREFIX)/lib/tcc/$(ARCH)-libtcc1.a

$(OBJFILES): $(BUILDDIR)/%.c.o: %.c
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<

$(OUTFILES): $(BUILDDIR)/%.h: %.h.in
	echo 'IO ' $<
	-mkdir -p $(shell dirname $@)
	cp $< $@
	-sed -i 's/__IN_ARCH_IN__/$(ARCH)/g' $@
