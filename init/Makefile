include ../makefiles/env.mk

# No all target
all:

.PHONY: all clean todolist builddir
.SILENT:

DIRS			:= src
OUTPUTDIR		:= build
TARGET			:= init.bin

CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-tcc
LD				:= $(PREFIX)/bin/$(ARCH)-tcc
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/src

$(TARGET): builddir $(COBJ)
	echo + Link $(TARGET)
	$(CC) -g -o $(TARGET) $(COBJ)
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > $(OUTPUTDIR)/LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -g -MD $(CC_WARNING) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true

clean:
	rm -f $(COBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)/LOC

# Header dependencies
-include $(DFILES)
