#include "../include/squire.h"

void squire_syscall_exit(int retval){
	squire_params_exit_t parms;
	parms.retval = retval;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_EXIT),"c"(sizeof(parms)),"d"(&parms));
}