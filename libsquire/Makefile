include ../makefiles/env.mk

# No all target
all:

.PHONY: all clean todolist builddir
.SILENT:

DIRS			:= src/$(ARCH)
OUTPUTDIR		:= build
TARGET			:= libsquire.a

CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ASMFILES		:= $(shell find $(DIRS) -type f -name "*.S")
ASMOBJ			:= $(ASMFILES:%.S=$(OUTPUTDIR)/%.S.o)

ALLFILES		:= $(ASMFILES) $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I $(PREFIX)/lib/tcc/include
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/src/$(ARCH)

$(TARGET): builddir $(COBJ) $(ASMOBJ)
	echo "LD  $(TARGET)"
	$(CC) -r -o $(TARGET) $(COBJ) $(ASMOBJ)
	find . -name '*.c' -o -name '*.h' | xargs wc -l > $(OUTPUTDIR)/LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) -g -MD -I src $(CC_WARNING) -o $@ -c $<

$(ASMOBJ): $(OUTPUTDIR)/%.S.o: %.S
	echo 'AS ' $<
	$(CC) -g -MD -I src $(CC_WARNIMG) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true

clean:
	rm -f $(COBJ) $(ASMOBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)/LOC

# Header dependencies
-include $(DFILES)
