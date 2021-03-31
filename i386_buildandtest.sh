#!/bin/sh

export PREFIX=$(pwd)/prefix
export PATH=$PREFIX/bin:$PATH

mkdir build >/dev/null 2>/dev/null
cd build
mkdir i386 >/dev/null 2>/dev/null

cd i386
#../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=i386-elf
make && make install
if test "X$?" != "X0" ; then
	echo "BUILD ERROR i386"
	cd ../..
	exit 1
fi
cd ..

cd ..

i386-elf-objdump -d build/i386/output/boot/squire > DISASS_I386

echo STARTING QEMU
qemu-system-i386 -kernel build/i386/output/boot/squire -serial mon:stdio -nographic -m 256M -no-reboot -d cpu_reset