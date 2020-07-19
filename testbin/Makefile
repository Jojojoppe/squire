.SILENT:

.PHONY: all clean todolist

DIRS			:= src
OUTPUTDIR		:= build

CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(CFILES) $(HFILES)

# TARGET ARCHITECTURE
ARCH			:= i386

# Toolchain
PTH				:= ../env/bin
TARGET			:= $(ARCH)-squire
PREFIX			:= $(PTH)/$(TARGET)

CC				:= $(PREFIX)-gcc
LD				:= $(PREFIX)-ld
OC				:= $(PREFIX)-objcopy
OD				:= $(PREFIX)-objdump
CC_WARNING		:= -Wall -Wextra

all:

testbin.bin: $(COBJ)
	echo + Link testbin.bin
	$(CC) -g -o testbin.bin $(COBJ)

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -g -MMD $(CC_WARNING) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -f $(COBJ) $(DFILES) testbin.bin DEBUG.sym DISASS

# Header dependencies
-include $(DFILES)
