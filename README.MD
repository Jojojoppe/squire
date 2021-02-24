# squire

Squire is a simple hobby OS with a microkernel currently only written for x86. Some notes on the implementation and usage of certain features can be found in the wiki (mostly written for  myself but feel free to take a look).

![Screenshot of serial terminal at squire boot](https://github.com/Jojojoppe/squire/raw/master/doc/img/screenshot_serial_0.png)

#### Building squire
Squire can be build with a simple `make all` command after cloning the repository. Things required for building

* a compiler capable of compiling TCC (any modern version of gcc should do the job). 
* a linux installation capable of running `dd`, `losetup`, `mkdosfs`, `mount/umount`, `sfdisk` and `grub-install`. `make drive/mount/umount` should be run as sudo since it uses losetup (or the right permissions should be set)

The build system needs two important system variables:

* `PREFIX` should be set to a directory in which the full toolchain and header files may be installed (make a directory in the cloned repo and use that)
* `ARCH` should be set to the architecture for which squire should be compiled. only `i386` is supported right now

#### Running squire
`make all` will generate a file called `drive` which can direcly be `dd`'ed to a drive to run physically or it can be used to run a VM. When QEMU is installed with the `qemu-system-i386` executable, `make run` will start QEMU and run squire inside QEMU. When no graphical interface is required `make runtty` will do the job. 

###### Used third party libraries and software:
* <b>TinyCC</b> TinyCC (TCC, version 0.9.27 commit df67d86) is used as compiler, assembler and linker for all the parts of squire. To fully support squire small changes are made to the TCC code base. The altered TCC is included in the repository of squire (tools/tcc). Changes made to TCC are stated in tools/tcc/SQUIRE_CHANGES.
* <b>PDCLib</b> PDCLib (the version of Erin Shepherd) is used as the userspace C standard library. Alterations and additions are made to support squire and it is included in the repository (libc).
* <b>FDLIBM</b> FDLIBM (version 5.3) is used as the standard C math library to extend PDCLib to complete the userspace C runtime

All licences of the third party libraries and software can be found in the licences folder.
###### LICENCE
squire is licenced under the BSD-2 licence which could be found in the LICENCE file.