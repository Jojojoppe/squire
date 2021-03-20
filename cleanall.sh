#!/bin/sh

export PREFIX=$(pwd)/prefix
export PATH=$PREFIX/bin:$PATH

mkdir build >/dev/null 2>/dev/null
cd build
mkdir arm i386 >/dev/null 2>/dev/null

cd arm
#../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=arm-none-eabi platform=ebaz4205
make clean
if test "X$?" != "X0" ; then
	echo "CLEAN ERROR arm"
	cd ../..
	exit 1
fi
cd ..

cd i386
#../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=i386-elf
make clean
if test "X$?" != "X0" ; then
	echo "CLEAN ERROR i386"
	cd ../..
	exit 1
fi
cd ..

cd ..
