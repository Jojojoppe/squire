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

init.bin: $(ASMOBJ) $(COBJ)
	echo + Link init.bin
	$(LD) -g -I inc -T linker.ld -o init.bin $(ASMOBJ) $(COBJ)
	$(OD) -d -M intel-mnemonic init.bin > DISASS
	$(OC) --only-keep-debug init.bin DEBUG.sym
	$(OC) --strip-debug init.bin
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > LOC

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -ffreestanding -g -MMD -I inc $(CC_WARNING) -o $@ -c $<

$(ASMOBJ): $(OUTPUTDIR)/%.asm.o: %.asm
	echo '>' $<
	$(AS) $(AS_FLAGS) -MD $(OUTPUTDIR)/$<.d -g -o $@ $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -f $(ASMOBJ) $(ASMDFILES) $(COBJ) $(DFILES) init.bin DEBUG.sym DISASS

# Header dependencies
-include $(ASMDFILES)
-include $(DFILES)
