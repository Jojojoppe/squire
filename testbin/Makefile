include ../makefiles/env.mk

# No all target
all:

.PHONY: all clean todolist builddir
.SILENT:

DIRS			:= src
OUTPUTDIR		:= build
TARGET			:= testbin.bin

CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-tcc
LD				:= $(CC)
OC				:= $(PREFIX)/bin/$(ARCH)-squire-objcopy
OD				:= $(PREFIX)/bin/$(ARCH)-squire-objdump
CC_WARNING		:= -Wall -Wextra

builddir:
	-mkdir -p $(OUTPUTDIR)/src

$(TARGET): builddir $(COBJ)
	echo + Link $(TARGET)
	$(CC) -g -o $(TARGET) $(COBJ)
	$(OD) -d -M intel-mnemonic $(TARGET) > $(OUTPUTDIR)/DISASS
	$(OC) --only-keep-debug $(TARGET) $(OUTPUTDIR)/DEBUG.sym
	$(OC) --strip-debug $(TARGET)
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > $(OUTPUTDIR)/LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -g -MD $(CC_WARNING) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -f $(COBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)/DEBUG.sym $(OUTPUTDIR)/DISASS $(OUTPUTDIR)/LOC

# Header dependencies
-include $(DFILES)
