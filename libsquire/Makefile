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

ALLFILES		:= $(ASMFILES) $(INCFILES) $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-tcc -nostdinc -nostdlib -I $(PREFIX)/lib/tcc/include
OC				:= $(PREFIX)/bin/$(ARCH)-squire-objcopy
OD				:= $(PREFIX)/bin/$(ARCH)-squire-objdump
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/src/$(ARCH)

$(TARGET): builddir $(COBJ)
	echo + Link $(TARGET)
	$(CC) -r -o $(TARGET) $(COBJ)
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > $(OUTPUTDIR)/LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -g -fPIC -MD -I src $(CC_WARNING) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -f $(COBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)/LOC

# Header dependencies
-include $(DFILES)
