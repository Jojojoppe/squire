#!/bin/sh

mkdir -p prefix/src prefix/build/gcc-i386 prefix/build/binutils-i386 prefix/build/binutils-arm prefix/build/gcc-arm
PROJECT_ROOT=$(pwd)
export PATH=$PROJECT_ROOT/bin:$PATH

# Downoad sources
#curl https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.xz > $PROJECT_ROOT/prefix/src/binutils.tar.gz
#curl https://ftp.gnu.org/gnu/gcc/gcc-11.2.0/gcc-11.2.0.tar.xz > $PROJECT_ROOT/prefix/src/gcc.tar.gz
# Untar sources
#cd $PROJECT_ROOT/prefix/src
#tar -xf binutils.tar.gz
#tar -xf gcc.tar.gz

# Build binutils for i386
#cd $PROJECT_ROOT/prefix/build/binutils-i386
#$PROJECT_ROOT/prefix/src/binutils*/configure --target=i386-elf --prefix=$PROJECT_ROOT/prefix --with-sysroot --disable-nls --disable-werror
#make -j2
#make install

# Build binutils for arm
#cd $PROJECT_ROOT/prefix/build/binutils-arm
#$PROJECT_ROOT/prefix/src/binutils*/configure --target=arm-none-eabi --prefix=$PROJECT_ROOT/prefix --with-sysroot --disable-nls --disable-werror
#make -j2
#make install

# Build gcc for i386
#cd $PROJECT_ROOT/prefix/build/gcc-i386
#$PROJECT_ROOT/prefix/src/gcc*/configure --target=i386-elf --prefix=$PROJECT_ROOT/prefix --disable-nls --enable-languages=c,c++ --without-headers
#make -j2 all-gcc
#make install-gcc
#make -j2 all-target-libgcc
#make install-target-libgcc

# Build gcc for arm
#cd $PROJECT_ROOT/prefix/build/gcc-arm
#$PROJECT_ROOT/prefix/src/gcc*/configure --target=arm-none-eabi --prefix=$PROJECT_ROOT/prefix --disable-nls --enable-languages=c,c++ --without-headers
#make -j2 all-gcc
#make install-gcc
#make -j2 all-target-libgcc
#make install-target-libgcc
