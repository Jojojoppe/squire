include makefiles/env.mk

.PHONY: all clean copy initramfs kernel init libsquire libc
.SILENT:

# Complete make
all: toolchain drive libsquire kernel libc init initramfs copy

# Makefile scripts
include makefiles/drive.mk
include makefiles/qemu.mk
include makefiles/toolchain.mk

# Clean all
clean:
	echo + Clean
	# goto subdirectories and clean there
	cd kernel && ${MAKE} ${MFLAGS} clean
	cd init && ${MAKE} ${MFLAGS} clean
	cd libsquire && ${MAKE} ${MFLAGS} clean
	cd libc && ${MAKE} ${MFLAGS} clean
	-rm initramfs.tar
	-rm -rf initramfs

# Copy files to drive
copy: mount
	echo + Copy files to drive
	sudo cp grub.cfg mnt/boot/grub/grub.cfg
	sudo cp kernel/kernel.bin mnt/boot/kernel.bin
	# sudo cp init/init.bin mnt/boot/init.bin
	# sudo cp initramfs.tar mnt/boot/initramfs.tar
	${MAKE} umount

# Compile the kernel
kernel:
	echo + Make kernel
	cd kernel && ${MAKE} ${MFLAGS} kernel.bin

# Compile libsquire
libsquire: libsquire_headers
	echo + Make libsquire
	cd libsquire && ${MAKE} ${MFLAGS} libsquire.a
	-mkdir -p $(PREFIX)/usr/lib
	cp libsquire/libsquire.a $(PREFIX)/usr/lib/libsquire.a
libsquire_headers:
	-mkdir -p $(PREFIX)/usr/include
	cp -RT libsquire/include $(PREFIX)/usr/include

libc:
	echo + Make libc
	cd libc && ${MAKE} ${MFLAGS} libc

# Compile init
init:
	echo + Make init
	cd init && ${MAKE} ${MFLAGS} init.bin

# Create initramfs
initramfs: testbin
	echo + Create initramfs
	-rm -rf initramfs
	mkdir initramfs
	# Make all entries
	cat initramfs.conf | xargs cp -t initramfs
	cd initramfs && tar -cf ../initramfs.tar *
