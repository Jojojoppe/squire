#include "../include/squire.h"

unsigned int squire_syscall_wait(unsigned int * retval, unsigned int pid){
	squire_params_wait_t parms;
    parms.pid = pid;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_WAIT),"c"(sizeof(parms)),"d"(&parms));
    *retval = parms.retval;
	return parms.reason;
}