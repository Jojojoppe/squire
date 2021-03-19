#!/bin/sh

export PREFIX=$(pwd)/prefix
export PATH=$PREFIX/bin:$PATH

mkdir build >/dev/null 2>/dev/null
cd build
mkdir arm i386 x86_64 >/dev/null 2>/dev/null

cd arm
../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=arm-none-eabi platform=ebaz4205
if test "X$?" != "X0" ; then
	echo "CONFIGURE ERROR arm"
	cd ../..
	exit 1
fi
#make && make install
cd ..

cd i386
../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=i386-elf
if test "X$?" != "X0" ; then
	echo "CONFIGURE ERROR i386"
	cd ../..
	exit 1
fi
#make && make install
cd ..

cd x86_64
../../configure --prefix=$PREFIX --exec-prefix=$(pwd)/output --host=x86_64-elf
if test "X$?" != "X0" ; then
	echo "CONFIGURE ERROR x86_64"
	cd ../..
	exit 1
fi
#make && make install
cd ..

cd ..
