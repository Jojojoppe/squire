#include "../include/squire.h"

uint32_t squire_syscall_thread(void (*entry)(void), void * stack, uint32_t flags){
	squire_params_thread_t parms;
	parms.entry = entry;
	parms.stack = stack;
	parms.flags = flags;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_THREAD),"c"(sizeof(parms)),"d"(&parms));
	return parms.entry;
}
