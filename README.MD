# squire

###### Used third party libraries and software:
* <b>TinyCC</b> TinyCC (TCC) is used as compiler, assembler and linker for all the parts of squire. To fully support squire small changes are made to the TCC code base. The altered TCC is included in the repository of squire (tools/tcc).
* <b>PDCLib</b> PDCLib (the version of Erin Shepherd) is used as the userspace C standard library. Alterations and additions are made to support squire and it is included in the repository (libc).
* <b>FDLIBM</b> FDLIBM (version 5.3) is used as the standard C math library to extend PDCLib to complete the userspace C runtime

All licences of the third party libraries and software can be found in the licences folder
