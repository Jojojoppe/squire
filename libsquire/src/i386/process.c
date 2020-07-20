#include "../include/squire.h"

uint32_t squire_syscall_process(void * elf_start, size_t elf_length){
	squire_params_process_t parms;
	parms.elf_start = elf_start;
	parms.elf_length = elf_length;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_PROCESS),"c"(sizeof(parms)),"d"(&parms));
	return parms.elf_start;
}
