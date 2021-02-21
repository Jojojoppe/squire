include ../../../makefiles/env.mk

TARGET				:= x86_generic.bin

BUILDDIR			:= build

INSTALLHEADER		:= $(PREFIX)/include/drivers

SRCDIRS				:= src
SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.o)
DEPFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.d)

INCFILES1			:= $(shell find include -type f -name "*.h")
INSFILES1			:= $(INCFILES1:include/%.h=$(INSTALLHEADER)/%.h)
INCFILES			:= $(INCFILES1)
INSFILES			:= $(INSFILES1)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)
FILESTOUNINSTALL	:= $(INSFILES)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:= -lm
CC_FLAGS			:= -g
CC_INCLUDES			:= -I src

LD_FLAGS			:= -g

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean install uninstall
all: $(TARGET) install

install: $(INSFILES)

uninstall:
	echo UNISTALL FILES FOR $(TARGET)
	-rm $(FILESTOUNINSTALL)

# PHONY RULES
# -----------

$(TARGET): $(BUILDDIR)/$(TARGET)

clean:
	echo CLEAN FILES FOR $(TARGET)
	-rm -r $(FILESTOCLEAN)

# SPECIFIC BUILD RULES
# --------------------

$(BUILDDIR)/$(TARGET): $(OBJFILES)
	echo 'LD ' $@
	-mkdir -p $(shell dirname $@)
	$(LD) $(LD_FLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJFILES) $(CC_LIBS)

$(OBJFILES): $(BUILDDIR)/%.c.o: %.c
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<

$(INSFILES1): $(INSTALLHEADER)/%.h: include/%.h
	echo 'CP ' $< $@
	-mkdir -p $(shell dirname $@)
	cp -RT $< $@
