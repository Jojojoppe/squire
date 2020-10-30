include ../makefiles/env.mk

# No all target
all:

.PHONY: all clean todolist builddir infiles
.SILENT:

SQUIRE_COMMIT  	:= $(shell git rev-parse --short HEAD)
SQUIRE_VMAJOR	:= v0.2-
SQUIRE_VERSION	:= $(SQUIRE_VMAJOR)$(SQUIRE_COMMIT)

DIRS			:= src/$(ARCH) src/general
OUTPUTDIR		:= build
TARGET			:= kernel.bin

ASMFILES		:= $(shell find $(DIRS) -type f -name "*.S")
ASMOBJ			:= $(ASMFILES:%.S=$(OUTPUTDIR)/%.S.o)

HINFILES		:= $(shell find $(DIRS) -type f -name "*.h.in")
INHFILES		:= $(HINFILES:%.h.in=$(OUTPUTDIR)/%.h)
CFILES			:= $(shell find $(DIRS) -type f -name "*.c")
HFILES			:= $(shell find $(DIRS) -type f -name "*.h") $(INHFILES) 
COBJ			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.o)
DFILES			:= $(CFILES:%.c=$(OUTPUTDIR)/%.c.d)

ALLFILES		:= $(ASMFILES) $(INCFILES) $(CFILES) $(HFILES)

# Toolchain
CC				:= $(PREFIX)/bin/$(ARCH)-tcc
CC_WARNING		:= -Wall -Wextra
CC_FLAGS		:= -Isrc -Ibuild/src -Isrc/general -nostdlib -nostdinc -O0 -g
CC_ARCH_FLAGS	:=
DEFS			:= -DSQUIRE_ARCH=\"$(ARCH)\" -DSQUIRE_VERSION=\"$(SQUIRE_VERSION)\"

include src/$(ARCH)/arch.mk

builddir:
	-mkdir -p $(OUTPUTDIR)/src/{general,$(ARCH)}
	-mkdir -p  $(OUTPUTDIR)/src/$(ARCH)/memory

$(TARGET): builddir $(INHFILES) $(ASMOBJ) $(COBJ)
	echo "LD  $(TARGET)"
	$(CC) $(CC_FLAGS) $(CC_ARCH_FLAGS) -static -o $(TARGET) $(DEFS) $(ASMOBJ) $(COBJ) $(PREFIX)/lib/tcc/$(ARCH)-libtcc1.a
	find . -name '*.c' -o -name '*.h' -o -name '*.asm' -o -name '*.inc' | xargs wc -l > $(OUTPUTDIR)/LOC

$(INHFILES): $(OUTPUTDIR)/%.h: %.h.in
	echo '>' $<
	cp $< $@
	-sed -i 's/__IN_ARCH_IN__/$(ARCH)/g' $@

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo 'CC ' $<
	$(CC) $(CC_FLAGS) $(DEFS) -MD $(CC_WARNING) -o $@ -c $<

$(ASMOBJ): $(OUTPUTDIR)/%.S.o: %.S
	echo 'AS ' $<
	$(CC) $(CC_FLAGS) $(DEFS) -MD $(CC_WARNING) -o $@ -c $<

todolist:
	echo + TODO list
	-@for file in $(ALLFILES:Makefile=); do fgrep -n -H -e TODO $$file ; done; true

clean:
	rm -rf $(ASMOBJ) $(ASMDFILES) $(COBJ) $(DFILES) $(TARGET) $(OUTPUTDIR)

# Header dependencies
-include $(ASMDFILES)
-include $(DFILES)
