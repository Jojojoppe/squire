include ../makefiles/env.mk

# No all target
all:

.PHONY: all clean todolist builddir infiles

SQUIRE_VERSION  := "v0.1-1"

DIRS			:= src/$(ARCH) src/general
OUTPUTDIR		:= build
TARGET			:= kernel.bin


ASMFILES	 	:= $(shell find $(DIRS) -type f -name "*.asm")
INCFILES	 	:= $(shell find $(DIRS) -type f -name "*.inc")
ASMOBJ			:= $(ASMFILES:%.asm=$(OUTPUTDIR)/%.asm.o)
ASMDFILES		:= $(ASMFILES:%.asm=$(OUTPUTDIR)/%.asm.d)

HINFILES		:= $(shell find $(DIRS) -type f -name "*.h.in")
INHFILES		:= $(HINFILES:%.h.in=$(OUTPUTDIR)/%.h)
CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h") $(INHFILES) 
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(ASMFILES) $(INCFILES) $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-squire-gcc
AS				:= nasm
LD				:= $(PREFIX)/bin/$(ARCH)-squire-gcc
OC				:= $(PREFIX)/bin/$(ARCH)-squire-objcopy
OD				:= $(PREFIX)/bin/$(ARCH)-squire-objdump
CC_WARNING		:= -Wall -Wextra
CC_FLAGS		:= -Isrc -Ibuild/src -mgeneral-regs-only -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow 
DEFS			:= -DSQUIRE_ARCH=\"$(ARCH)\" -DSQUIRE_VERSION=\"$(SQUIRE_VERSION)\"

include src/$(ARCH)/arch.mk

builddir:
	-mkdir -p $(OUTPUTDIR)/src/{general,$(ARCH)}
	-mkdir -p  $(OUTPUTDIR)/src/$(ARCH)/memory

$(TARGET): builddir $(INHFILES) $(ASMOBJ) $(COBJ)
	echo + Link $(TARGET)
	$(LD) -g -T src/$(ARCH)/linker.ld -o $(TARGET) $(DEFS) $(ASMOBJ) $(COBJ) -nostdlib -lgcc
	$(OD) -d -M intel-mnemonic $(TARGET) > $(OUTPUTDIR)/DISASS
	$(OC) --only-keep-debug $(TARGET) $(OUTPUTDIR)/DEBUG.sym
	$(OC) --strip-debug $(TARGET)
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > $(OUTPUTDIR)/LOC

$(INHFILES): $(OUTPUTDIR)/%.h: %.h.in
	echo '>' $<
	cp $< $@
	-sed -i 's/__IN_ARCH_IN__/$(ARCH)/g' $@

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) $(CC_FLAGS) $(DEFS) -ffreestanding -g -MMD $(CC_WARNING) -o $@ -c $<

$(ASMOBJ): $(OUTPUTDIR)/%.asm.o: %.asm
	echo '>' $<
	$(AS) $(AS_FLAGS) -MD $(OUTPUTDIR)/$<.d -g -o $@ $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true
	#| sed -e 's/:[^:]*//2g' | sed -e 's/^..//' | awk '{print ">"$$1 }' ; fgrep -n -H -e TODO $$file | sed -e 's#.*TODO\(\)#\    #'; done; true

clean:
	rm -rf $(ASMOBJ) $(ASMDFILES) $(COBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)

# Header dependencies
-include $(ASMDFILES)
-include $(DFILES)
