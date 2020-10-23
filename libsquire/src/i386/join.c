#include "../include/squire.h"

int squire_syscall_join(unsigned int id){
	squire_params_join_t parms;
    parms.id = id;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_JOIN),"c"(sizeof(parms)),"d"(&parms));
	return parms.retval;
}

