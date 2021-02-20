include ../makefiles/env.mk

TARGET				:= libsquire.a

BUILDDIR			:= build

SRCDIRS				:= src
SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.o)
DEPFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.d)

INSTALLTARGET		:= $(PREFIX)/lib
INSTALLHEADER		:= $(PREFIX)/include

INCFILES1			:= $(shell find include -type f -name "*.h")
INSFILES1			:= $(INCFILES1:include/%.h=$(INSTALLHEADER)/%.h)
INCFILES			:= $(INCFILES1)
INSFILES			:= $(INSFILES1)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)
FILESTOUNINSTALL	:= $(INSFILES) $(INSTALLTARGET)/$(TARGET)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:=
CC_FLAGS			:= -nostdlib
CC_INCLUDES			:= -I include -I $(PREFIX)/include

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean install uninstall
all: $(TARGET) install

# PHONY RULES
# -----------

$(TARGET): $(BUILDDIR)/$(TARGET)

clean:
	echo CLEAN FILES FOR $(TARGET)
	-rm -r $(FILESTOCLEAN)

install: $(INSTALLTARGET)/$(TARGET) $(INSFILES)

uninstall:
	echo UNISTALL FILES FOR $(TARGET)
	-rm $(FILESTOUNINSTALL)

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

$(INSFILES): $(INSTALLHEADER)/%.h: include/%.h
	echo 'CP ' $< $@
	-mkdir -p $(shell dirname $@)
	cp -RT $< $@
