.SILENT:

.PHONY: all clean run reset clean mount umount kernel debug env runtty TODO

# Normal list of actions
all: drive kernel mount copy umount

# Make drive
drive:
	echo + Make drive
	dd if=/dev/zero status=progress of=drive bs=512 count=131072
	echo + Write partitions
	( echo 2048,,L,* ) | sfdisk drive
	echo + Mount partitions
	mkdir mnt
	sudo losetup /dev/loop0 drive
	sudo losetup /dev/loop1 drive -o 1048576
	sudo mkdosfs -F32 -f 2 /dev/loop1
	sudo mount /dev/loop1 mnt
	sudo mkdir mnt/boot
	echo + Install GRUB
	sudo grub-install --target=i386-pc --boot-directory=mnt/boot --modules "normal part_msdos ext2 multiboot" --force /dev/loop0
	echo + Finish
	sync
	sudo umount mnt
	sudo losetup -d /dev/loop1
	sudo losetup -d /dev/loop0
	rmdir mnt

# Reset all
# note: this will delete the drive with all its files
reset:
	echo + Remove drive
	-rm drive
	-rm -rf mnt
	${MAKE} clean

# Clean all
clean:
	echo + Clean
	# goto subdirectories and clean there
	cd kernel && ${MAKE} clean

# Mount drive
mount:
	echo + Mount drive
	sudo losetup /dev/loop0 drive
	sudo losetup /dev/loop1 drive -o 1048576
	mkdir mnt
	sudo mount /dev/loop1 mnt

# Unmount drive
umount:
	echo + Unmount drive
	sync
	sudo umount mnt
	sudo losetup -d /dev/loop1
	sudo losetup -d /dev/loop0
	rmdir mnt

# Copy files to drive
copy:
	echo + Copy files to drive
	sudo cp grub.cfg mnt/boot/grub/grub.cfg
	sudo cp kernel/kernel.bin mnt/boot/kernel.bin

# Run the test suite
run:
	echo + Run test suite
	qemu-system-i386 -s -m 32M -no-reboot -drive file=drive,format=raw -monitor stdio

# Run the test suite
runtty:
	echo + Run test suite in terminal
	qemu-system-i386 -s -m 32M -no-reboot -drive file=drive,format=raw -nographic

# Debug
debug:
	echo + Debug
	qemu-system-i386 -s -S -m 32M -no-shutdown -no-reboot -drive file=drive,format=raw &
	sleep 1
	gdb -x kernel/DEBUG

# Compile the kernel
kernel:
	echo + Make kernel
	cd kernel && ${MAKE} ${MFLAGS} kernel.bin

# List all todos
TODO:
	cd kernel && ${MAKE} ${MFLAGS} todolist

# Create environment with toolchain
# This will install binutils and gcc to env/cross
PREFIX		= "$(shell pwd)/env/cross"
TARGET		= "i386-elf"
env:
	-mkdir env
	-mkdir env/cross
	-rm -rf env/cross/*
	-mkdir env/cross/src
	PATH="$(PREFIX)/bin:${PATH}"

	# Make BINUTILS
	-mkdir env/cross/src/binutils
	curl https://ftp.gnu.org/gnu/binutils/binutils-2.31.tar.gz > env/cross/src/binutils.tar.gz
	tar -xvf env/cross/src/binutils.tar.gz -C env/cross/src/binutils --strip-components=1
	mkdir env/cross/src/build-binutils
	cd env/cross/src/build-binutils; \
	../binutils/configure --target=$(TARGET) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror; \
	make; \
	make install

	# Make GCC
	-mkdir env/cross/src/gcc
	curl https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz > env/cross/src/gcc.tar.gz
	tar -xvf env/cross/src/gcc.tar.gz -C env/cross/src/gcc --strip-components=1
	mkdir env/cross/src/build-gcc
	cd env/cross/src/build-gcc; \
	../gcc/configure --target=$(TARGET) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers; \
	make all-gcc; \
	make all-target-libgcc; \
	make install-gcc; \
	make install-target-libgcc
