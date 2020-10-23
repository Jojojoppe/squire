#include "../include/squire.h"

void squire_thread_end(){
	int eax;
	__asm__ __volatile__("nop":"=a"(eax));
	squire_syscall_exit(eax);
	for(;;);
}

uint32_t squire_syscall_thread(void (*entry)(void), void * stack_base, size_t stack_length, uint32_t flags, void * param){
	squire_params_thread_t parms;
	parms.entry = entry;
	parms.stack_base = stack_base;
	parms.stack_length = stack_length;
	parms.flags = flags;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_THREAD),"c"(sizeof(parms)),"d"(&parms));
	unsigned int * s = (unsigned int*)(stack_base + stack_length);
	*(s-1) = squire_thread_end;
	*(s) = (unsigned int) param;
	return parms.entry;
}