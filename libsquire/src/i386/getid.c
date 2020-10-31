#include "../include/squire.h"

extern void squire_syscall_getid(unsigned int * pid, unsigned int * tid){
	squire_params_getid_t parms;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_GETID),"c"(sizeof(parms)),"d"(&parms));
    *pid = parms.pid;
    *tid = parms.tid;
}