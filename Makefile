# SQUIRE MAIN MAKEFILE

.PHONY: all clean copy

# Complete make
all: env_check newlib drive kernel/kernel.bin libsquire/libsquire.a init/init.bin initramfs.tar copy

# Makefile scripts
include makefiles/env.mk
include makefiles/drive.mk
include makefiles/toolchain.mk
include makefiles/qemu.mk

# Clean all
clean:
	echo + Clean
	# goto subdirectories and clean there
	cd kernel && ${MAKE} ${MFLAGS} clean
	cd init && ${MAKE} ${MFLAGS} clean
	cd testbin && ${MAKE} ${MFLAGS} clean
	-rm initramfs.tar
	-rm -rf initramfs

# Copy files to drive
copy: mount
	echo + Copy files to drive
	sudo cp grub.cfg mnt/boot/grub/grub.cfg
	sudo cp kernel/kernel.bin mnt/boot/kernel.bin
	sudo cp init/init.bin mnt/boot/init.bin
	sudo cp initramfs.tar mnt/boot/initramfs.tar
	${MAKE} umount

# Compile the kernel
kernel/kernel.bin: env_check
	echo + Make kernel
	cd kernel && ${MAKE} ${MFLAGS} kernel.bin

# Compile libsquire
libsquire/libsquire.a: env_check libsquire_headers
	echo + Make libsquire
	cd libsquire && ${MAKE} ${MFLAGS} libsquire.a
	cp libsquire/libsquire.a $(PREFIX)/usr/lib
libsquire_headers: env_check
	cp -RT libsquire/include $(PREFIX)/usr/include

# Compile newlib
newlib: env_check libsquire_headers
	echo + Make newlib
	${MAKE} ${MFLAGS} newlib_hosted

# Compile init
init/init.bin: env_check
	echo + Make init
	cd init && ${MAKE} ${MFLAGS} init.bin

# Create initramfs
initramfs.tar: testbin/testbin.bin
	echo + Create initramfs
	-rm -rf initramfs
	mkdir initramfs
	# Make all entries
	cat initramfs.conf | xargs cp -t initramfs
	cd initramfs && tar -cf ../initramfs.tar *

# Other programs
testbin/testbin.bin: env_check
	echo + Create testbin
	cd testbin && ${MAKE} ${MFLAGS} testbin.bin
