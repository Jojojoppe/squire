#include "../include/squire.h"

void squire_syscall_kill(unsigned int pid){
	squire_params_wait_t parms;
    parms.pid = pid;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_KILL),"c"(sizeof(parms)),"d"(&parms));
}