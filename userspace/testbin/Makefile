include ../../makefiles/env.mk

.PHONY: all clean builddir
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
CC_WARNING		:= -Wall -Wextra
CC_LIBS			:= 

# No all target
all: $(TARGET)

builddir:
	-mkdir -p $(OUTPUTDIR)/src

$(TARGET): builddir $(COBJ)
	echo + Link $(TARGET)
	$(CC) -o $(TARGET) $(COBJ)

$(COBJ): $(OUTPUTDIR)/%.c.o: %.c
	echo '>' $<
	$(CC) -MD $(CC_WARNING) -o $@ -c $<

clean:
	rm -f $(COBJ) $(DFIELS) $(TARGET)

# Header dependencies
-include $(DFILES)
