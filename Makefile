#.SILENT:

.PHONY: all clean run reset clean mount umount kernel debug env runtty TODO init initramfs

# Normal list of actions
all: drive kernel init initramfs mount copy umount TODO

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
clean: initramfsclean
	echo + Clean
	# goto subdirectories and clean there
	cd kernel && ${MAKE} clean
	cd init && ${MAKE} clean

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
	sudo cp init/init.bin mnt/boot/init.bin
	sudo cp initramfs.tar mnt/boot/initramfs.tar

# Run the test suite
run:
	echo + Run test suite
	qemu-system-i386 -s -m 64M -no-reboot -drive file=drive,format=raw -monitor stdio

# Run the test suite
runtty:
	echo + Run test suite in terminal
	qemu-system-i386 -s -m 64M -no-reboot -drive file=drive,format=raw -nographic

# Debug
debug:
	echo + Debug
	qemu-system-i386 -s -S -m 64M -no-shutdown -no-reboot -drive file=drive,format=raw &
	sleep 1
	gdb -x kernel/DEBUG

# Compile the kernel
kernel:
	echo + Make kernel
	cd kernel && ${MAKE} ${MFLAGS} kernel.bin

# Compile init
init:
	echo + Make init
	cd init && ${MAKE} ${MFLAGS} init.bin

# Create initramfs
initramfs:
	echo + Create initramfs
	-rm -rf initramfs
	mkdir initramfs
	# Make all entries
	cd testbin && ${MAKE} ${MFLAGS} testbin.bin
	cat initramfs.conf | xargs cp -t initramfs
	cd initramfs && tar -cf ../initramfs.tar *
# Destroy initramfs
initramfsclean:
	-rm initramfs.tar
	-rm -rf initramfs
	# Clean all entries
	cd testbin && ${MAKE} clean


# List all todos
TODO:
	cd kernel && ${MAKE} ${MFLAGS} todolist

# Create environment with toolchain
# This will install binutils and gcc to env
env: env_cross env_newlib
PREFIX		= $(shell pwd)/env
CROSSTARGET	= "i386-elf"
TARGET		= "i386-squire"

env_cross:
	-rm -rf env
	-mkdir env
	-mkdir env/src
	$(eval export PATH=$(PREFIX)/bin:$(PATH))

	# Make cross BINUTILS
	-mkdir env/src/binutils
	curl https://ftp.gnu.org/gnu/binutils/binutils-2.31.tar.gz > env/src/binutils.tar.gz
	tar -xvf env/src/binutils.tar.gz -C env/src/binutils --strip-components=1
	mkdir env/src/build-binutils
	cd env/src/build-binutils; \
	../binutils/configure --target=$(CROSSTARGET) --prefix="$(PREFIX)" --with-sysroot --disable-nls --disable-werror; \
	make; \
	make install

	# Make cross GCC
	-mkdir env/src/gcc
	curl https://ftp.gnu.org/gnu/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz > env/src/gcc.tar.gz
	tar -xvf env/src/gcc.tar.gz -C env/src/gcc --strip-components=1
	mkdir env/src/build-gcc
	cd env/src/build-gcc; \
	../gcc/configure --target=$(CROSSTARGET) --prefix="$(PREFIX)" --disable-nls --enable-languages=c,c++ --without-headers; \
	make all-gcc; \
	make all-target-libgcc; \
	make install-gcc; \
	make install-target-libgcc

	# Make automake
	-mkdir env/src/automake
	curl https://ftp.gnu.org/gnu/automake/automake-1.11.tar.gz > env/src/automake.tar.gz
	tar -xvf env/src/automake.tar.gz -C env/src/automake --strip-components=1
	mkdir env/src/build-automake
	cd env/src/build-automake; \
	../automake/configure --prefix="$(PREFIX)"; \
	make; \
	make install

	# Make autoconf
	-mkdir env/src/autoconf
	curl https://ftp.gnu.org/gnu/autoconf/autoconf-2.65.tar.gz > env/src/autoconf.tar.gz
	tar -xvf env/src/autoconf.tar.gz -C env/src/autoconf --strip-components=1
	mkdir env/src/build-autoconf
	cd env/src/build-autoconf; \
	../autoconf/configure --prefix="$(PREFIX)"; \
	make; \
	make install

env_newlib:
	-rm -rf env/src/newlib
	-rm -rf env/src/build-newlib
	$(eval export PATH=${PREFIX}/bin:$(PATH))

	# Setup newlib
	-mkdir env/src/newlib
	#curl ftp://sourceware.org/pub/newlib/newlib-2.5.0.tar.gz > env/src/newlib.tar.gz
	tar -xvf env/src/newlib.tar.gz -C env/src/newlib --strip-components=1
	# Apply changes
	patch env/src/newlib/config.sub < envbuild/newlib/config_sub.diff
	patch env/src/newlib/newlib/configure.host < envbuild/newlib/configure_host.diff
	patch env/src/newlib/newlib/libc/sys/configure.in < envbuild/newlib/libc_sys_configure_in.diff
	cd env/src/newlib/newlib/libc/sys && autoconf
	mkdir env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/configure.in env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/Makefile.am env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/crt0.c env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/syscalls.c env/src/newlib/newlib/libc/sys/squire
	# Configure newlib structure
	cd env/src/newlib/newlib/libc/sys && autoconf
	cd env/src/newlib/newlib/libc/sys/squire && autoreconf
	# Create temporary os compilers
	ln -sf $(PREFIX)/bin/i386-elf-ar $(PREFIX)/bin/i386-squire-ar
	ln -sf $(PREFIX)/bin/i386-elf-gcc $(PREFIX)/bin/i386-squire-gcc
	ln -sf $(PREFIX)/bin/i386-elf-gcc $(PREFIX)/bin/i386-squire-cc
	ln -sf $(PREFIX)/bin/i386-elf-as $(PREFIX)/bin/i386-squire-as
	ln -sf $(PREFIX)/bin/i386-elf-ranlib $(PREFIX)/bin/i386-squire-ranlib
	mkdir env/src/build-newlib
	cd env/src/build-newlib; \
	../newlib/configure --prefix="$(PREFIX)" --target=$(TARGET); \
	make all; \
	make install
#TESTING directly rebuild newlib
env_rebuild_newlib:
	cp envbuild/newlib/configure.in env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/Makefile.am env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/crt0.c env/src/newlib/newlib/libc/sys/squire
	cp envbuild/newlib/syscalls.c env/src/newlib/newlib/libc/sys/squire
	# Configure newlib structure
	cd env/src/build-newlib; \
	make all -O0; \
	make uninstall; \
	make install
