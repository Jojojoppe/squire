include makefiles/env.mk

.PHONY: all libsquire libc libm kernel init userspace initramfs install clean
.SILENT:

# Complete make
all: toolchain drive libsquire libc libm kernel init userspace initramfs mount install umount

# Makefile scripts
include makefiles/drive.mk
include makefiles/qemu.mk
include makefiles/toolchain.mk

# TARGETS
libsquire:
	cd libsquire && ${MAKE} ${MFLAGS} all

libc:
	cd libc && ${MAKE} ${MFLAGS} all

libm:
	cd libm && ${MAKE} ${MFLAGS} all

kernel:
	cd kernel && ${MAKE} ${MFLAGS} kernel.bin

init:
	cd init && ${MAKE} ${MFLAGS} init.bin

userspace:
	cd userspace && ${MAKE} ${MFLAGS} all

# INSTALL
install:
	rm mnt/boot/{kernel.bin,init.bin,initramfs.tar}
	cd kernel && ${MAKE} ${MFLAGS} install
	cd init && ${MAKE} ${MFLAGS} install
	cp initramfs.tar mnt/boot/initramfs.tar

# CLEAN
clean:
	cd libsquire && ${MAKE} ${MFLAGS} clean
	cd libc && ${MAKE} ${MFLAGS} clean
	cd libm && ${MAKE} ${MFLAGS} clean
	cd kernel && ${MAKE} ${MFLAGS} clean
	cd init && ${MAKE} ${MFLAGS} clean
	cd userspace && ${MAKE} ${MFLAGS} clean
	-rm initramfs.tar

# INITRAMFS
initramfs:
	echo + Create initramfs
	-rm -rf initramfs
	mkdir initramfs
	# Make all entries
	cat initramfs.conf | xargs -t cp -t initramfs
	cd initramfs && tar -cf ../initramfs.tar *
