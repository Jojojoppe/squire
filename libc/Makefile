include ../makefiles/env.mk

TARGET				:= libc.a

BUILDDIR			:= build

SRCDIRS				:= functions
SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.o)
DEPFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.d)

INSTALLTARGET		:= $(PREFIX)/lib
INSTALLHEADER		:= $(PREFIX)/include

INCFILES1			:= $(shell find include -type f -name "*.h")
INSFILES1			:= $(INCFILES1:include/%.h=$(INSTALLHEADER)/%.h)
INCFILES2			:= $(shell find internals -type f -name "*.h")
INSFILES2			:= $(INCFILES2:internals/%.h=$(INSTALLHEADER)/%.h)
INCFILES			:= $(INCFILES1) $(INCFILES2)
INSFILES			:= $(INSFILES1) $(INSFILES2)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)
FILESTOUNINSTALL	:= $(INSFILES) $(INSTALLTARGET)/$(TARGET)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:=
CC_FLAGS			:= -nostdlib -D_PDCLIB_EXTENSIONS
CC_INCLUDES			:= -I include -I internals

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean install uninstall crtfiles
all: $(TARGET) install crtfiles

# PHONY RULES
# -----------

$(TARGET): $(BUILDDIR)/$(TARGET)

clean:
	echo CLEAN FILES FOR $(TARGET)
	-rm -r $(FILESTOCLEAN)
	# Subfolders
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} clean

install: $(INSTALLTARGET)/$(TARGET) $(INSFILES)
	# Subfolders
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} install

uninstall:
	echo UNINSTALL FILES FOR $(TARGET)
	-rm $(FILESTOUNINSTALL)
	# Subfolders
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} uninstall

# Subfolders
crtfiles:
	cd platform/$(ARCH) && ${MAKE} ${MFLAGS} crtfiles

# SPECIFIC BUILD RULES
# --------------------

$(INSTALLTARGET)/$(TARGET): $(BUILDDIR)/$(TARGET)
	echo 'CP ' $(BUILDDIR)/$(TARGET) $(INSTALLTARGET)/$(TARGET)
	cp $(BUILDDIR)/$(TARGET) $(INSTALLTARGET)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJFILES)
	echo 'AR ' $@
	$(AR) rcs $(BUILDDIR)/$(TARGET) $(OBJFILES)

$(OBJFILES): $(BUILDDIR)/%.c.o: %.c
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<

$(INSFILES1): $(INSTALLHEADER)/%.h: include/%.h
	echo 'CP ' $< $@
	-mkdir -p $(shell dirname $@)
	cp -RT $< $@
$(INSFILES2): $(INSTALLHEADER)/%.h: internals/%.h
	echo 'CP ' $< $@
	-mkdir -p $(shell dirname $@)
	cp -RT $< $@
