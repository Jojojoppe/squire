CC_ARCH_FLAGS	:= -Wl,-Ttext=0xc0100000,-physoffset=-0xc0000000

AS_FLAGS		:= -f elf -isrc/$(ARCH)