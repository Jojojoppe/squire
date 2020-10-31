include ../../../makefiles/env.mk

TARGET				:= libarch.o

BUILDDIR			:= ../../build/src/i386

SRCDIRS				:=
SRCFILESC			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILESC			:= $(SRCFILESC:%.c=$(BUILDDIR)/%.c.o)
DEPFILESC			:= $(SRCFILESC:%.c=$(BUILDDIR)/%.c.d)
SRCFILESA			:= $(shell find $(SRCDIRS) -type f -name "*.S")
OBJFILESA			:= $(SRCFILESA:%.S=$(BUILDDIR)/%.S.o)
DEPFILESA			:= $(SRCFILESA:%.S=$(BUILDDIR)/%.S.d)
SRCFILES			:= $(SRCFILESC) $(SRCFILESA)
OBJFILES			:= $(OBJFILESA) $(OBJFILESC)
DEPFILES			:= $(DEPFILESC) $(DEPFILESA)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:= $(BUILDDIR)/libarch.a
CC_FLAGS			:= -nostdlib -nostdinc -O0 -g
CC_INCLUDES			:= -I ../../src -I ../../src/general -I ../../build/src -I ../../../libsquire/include

# Add DEPFILE dependencies
-include $(DEPFILES)

.PHONY: $(TARGET) all clean
all: $(TARGET)

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
	$(LD) -r -o $(BUILDDIR)/$(TARGET) $(OBJFILES)

$(OBJFILESC): $(BUILDDIR)/%.c.o: %.c
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<

$(OBJFILESA): $(BUILDDIR)/%.S.o: %.S
	echo 'AS ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<
