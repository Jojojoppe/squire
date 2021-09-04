noinst_LIBRARIES = libarch.a
libarch_adir = .
noinst_DATA = linker.ld
libarch_a_SOURCES = \
	boot.S \
	spinlock.S \
	archmain.c \
	$(PLATFORM_DIR)/debugprint.c \
	pmm.h pmm.c $(PLATFORM_DIR)/memory_layout.c \
	vas.h vas.c \
	clocks.c \
	gic.c gic.h \
	cpu.h cpu.c

linker.ld: $(PLATFORM_DIR)/linker.ld
	$(AM_V_at)rm -f $@
	$(AM_V_GEN)$(LN_S) $< $@ >/dev/null 2>/dev/null || cp $< $@
$(PLATFORM_DIR)/linker.ld:
