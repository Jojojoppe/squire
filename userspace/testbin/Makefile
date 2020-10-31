include ../../makefiles/env.mk

TARGET				:= testbin.bin

BUILDDIR			:= build

SRCDIRS				:= src
SRCFILES			:= $(shell find $(SRCDIRS) -type f -name "*.c")
OBJFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.o)
DEPFILES			:= $(SRCFILES:%.c=$(BUILDDIR)/%.c.d)

FILESTOCLEAN		:= $(OBJFILES) $(DEPFILES) $(BUILDDIR)/$(TARGET)

CC					:= $(PREFIX)/bin/$(ARCH)-tcc
AR					:= $(PREFIX)/bin/$(ARCH)-tcc -ar
LD					:= $(PREFIX)/bin/$(ARCH)-tcc
AS					:= $(PREFIX)/bin/$(ARCH)-tcc

CC_WARNING			:= -Wall -Wextra
CC_LIBS				:= -lm
CC_FLAGS			:=
CC_INCLUDES			:= -I src

LD_FLAGS			:=

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
	-mkdir -p $(shell dirname $@)
	$(LD) $(LD_FLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJFILES) $(CC_LIBS)

$(OBJFILES): $(BUILDDIR)/%.c.o: %.c
	echo 'CC ' $@
	-mkdir -p $(shell dirname $@)
	$(CC) $(CC_FLAGS) $(CC_WARNING) $(CC_INCLUDES) -MD -o $@ -c $<
