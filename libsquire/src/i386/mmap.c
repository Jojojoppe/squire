#include "../include/squire.h"

void * squire_syscall_mmap(void * address, size_t length, uint32_t flags){
	squire_params_mmap_t parms;
	parms.address = address;
	parms.length = length;
	parms.flags = flags;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_MMAP),"c"(sizeof(parms)),"d"(&parms));
	return parms.address;
}
