.SILENT:

.PHONY: all clean todolist

DIRS			:= src
OUTPUTDIR		:= build

ASMFILES	 	:= $(shell find $(DIRS) -type f -name "*.asm")
INCFILES	 	:= $(shell find $(DIRS) -type f -name "*.inc")
ASMOBJ			:= $(ASMFILES:%.asm=$(OUTPUTDIR)/%.asm.o)
ASMDFILES		:= $(ASMFILES:%.asm=$(OUTPUTDIR)/%.asm.d)

CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h")
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(ASMFILES) $(INCFILES) $(CFILES) $(HFILES)

# TARGET ARCHITECTURE
ARCH			:= "i386"

# Toolchain
PTH				:= "../env/bin"
TARGET			:= "$(ARCH)-elf"
PREFIX			:= "$(PTH)/$(TARGET)"

CC				:= "$(PREFIX)-gcc"
AS				:= "nasm"
LD				:= "$(PREFIX)-ld"
OC				:= "$(PREFIX)-objcopy"
OD				:= "$(PREFIX)-objdump"
CC_WARNING		:= -Wall -Wextra
AS_FLAGS		:= -f elf -isrc

all:

testbin.bin: $(ASMOBJ) $(COBJ)
	echo + Link testbin.bin
	$(LD) -g -I/home/joppe/Documents/Projects/Assembly/squire/env/i386-squire/include -T linker.ld -L/home/joppe/Documents/Projects/Assembly/squire/env/i386-squire/lib -o testbin.bin /home/joppe/Documents/Projects/Assembly/squire/env/i386-squire/lib/crt0.o $(ASMOBJ) $(COBJ) -lc -g -lm -lnosys
	$(OD) -d -M intel-mnemonic testbin.bin > DISASS
	$(OC) --only-keep-debug testbin.bin DEBUG.sym
	$(OC) --strip-debug testbin.bin
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -ffreestanding -g -MMD -I/home/joppe/Documents/Projects/Assembly/squire/env/i386-squire/include $(CC_WARNING) -o $@ -c $<

$(ASMOBJ): $(OUTPUTDIR)/%.asm.o: %.asm
	echo '>' $<
	$(AS) $(AS_FLAGS) -MD $(OUTPUTDIR)/$<.d -g -o $@ $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -f $(ASMOBJ) $(ASMDFILES) $(COBJ) $(DFILES) testbin.bin DEBUG.sym DISASS

# Header dependencies
-include $(ASMDFILES)
-include $(DFILES)
