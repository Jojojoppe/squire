SUBDIRS = $(ARCH_DIR)
DIST_SUBDIRS = src/arch/arm \
			   src/arch/i386

bootdir = @bootdir@
boot_PROGRAMS = squire
squire_LDADD = $(ARCH_DIR)/libarch.a
squire_LDFLAGS = -Tlinker.ld
squire_CFLAGS = $(extracflags)
noinst_DATA = linker.ld
EXTRA_squire_DEPENDENCIES = linker.ld
squire_SOURCES = \
	src/common/main.c \
	src/common/kprintf.c src/common/kprintf.h \
	src/common/arch/spinlock.h \
	src/common/arch/debugprint.h \
	src/common/arch/pmm.h \
	src/common/kmalloc.c src/common/kmalloc.h \
	src/common/arch/vas.h \
	src/common/arch/clocks.h src/common/sysclock.c src/common/sysclock.h

include_HEADERS = include/squire/squire.h

linker.ld : $(ARCH_DIR)/linker.ld
	$(AM_V_at)rm -f $@
	$(AM_V_GEN)$(LN_S) $< $@ >/dev/null 2>/dev/null || cp $< $@
$(ARCH_DIR)/linker.ld:
