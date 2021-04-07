#!/bin/sh

export PREFIX=$(pwd)/prefix
export PATH=$PREFIX/bin:$PATH

mkdir build >/dev/null 2>/dev/null
cd build
mkdir arm >/dev/null 2>/dev/null

cd arm
#../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=arm-none-eabi platform=ebaz4205
make && make install
if test "X$?" != "X0" ; then
	echo "BUILD ERROR arm"
	cd ../..
	exit 1
fi
cd ..

cd ..

arm-none-eabi-objdump -d build/arm/output/boot/squire > DISASS_ARM
arm-none-eabi-readelf -a build/arm/output/boot/squire >> DISASS_ARM

echo STARTING QEMU
qemu-system-arm -machine xilinx-zynq-a9 -kernel build/arm/output/boot/squire -serial /dev/null -serial mon:stdio -nographic -m 256M