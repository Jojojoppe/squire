#include "../include/squire.h"

void squire_thread_end(){
	squire_syscall_exit(0);
	for(;;);
}

uint32_t squire_syscall_thread(void (*entry)(void), void * stack_base, size_t stack_length, uint32_t flags){
	squire_params_thread_t parms;
	parms.entry = entry;
	parms.stack_base = stack_base;
	parms.stack_length = stack_length;
	parms.flags = flags;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_THREAD),"c"(sizeof(parms)),"d"(&parms));
	unsigned int * s = (unsigned int*)(stack_base + stack_length);
	*(s-1) = squire_thread_end;
	return parms.entry;
}

void squire_syscall_exit(int retval){
	squire_params_exit_t parms;
	parms.retval = retval;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_EXIT),"c"(sizeof(parms)),"d"(&parms));
}