# squire

Squire is a simple hobby OS with a microkernel currently only written for x86. Some notes on the implementation and usage of certain features can be found in the wiki (mostly written for  myself but feel free to take a look).

#### Building squire
The cross-compiler needed to build squire can be built with `./buildtools.sh`. Comment out the sections which are not needed when building just for one platform. Building squire itself can be done manually with a simple `configure --prefix=FOLDER_WITH_TOOLCHAIN --exec-prefix=FOLDER_FOR_OUTPUT_AFTER_INSTALL --host=HOST_TYPE platform=SPECIFIC_PLATFORM`. When the host type is `arm-none-eabi` a specific platform must be selected (not needed for `i386-elf`). Currently only `ebaz4205` is supported. Then a make/make install combination should do the job. Scripts for this are also provided: `cleanall.sh`, `configureall.sh` and `i386_buildandtest.sh` or `arm_buildandtest.sh`.

#### Running squire
The build and test scripts will run qemu connected to the command line. One could put the created ELF on a disk with a bootloader as well.

###### LICENCE
squire is licenced under the BSD-2 licence which could be found in the LICENCE file.
