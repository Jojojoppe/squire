#include "../include/squire.h"

void squire_syscall_kill(unsigned int pid, unsigned int reason){
	squire_params_wait_t parms;
    parms.pid = pid;
	parms.reason = reason;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_KILL),"c"(sizeof(parms)),"d"(&parms));
}