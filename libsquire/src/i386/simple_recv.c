#include "../include/squire.h"

unsigned int squire_syscall_simple_recv(void * buffer, size_t * length, unsigned int * from){
    squire_params_simple_recv_t parms;
    parms.from = from;
    parms.length = length;
    parms.buffer = buffer;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_SIMPLE_RECV),"c"(sizeof(parms)),"d"(&parms));
    return parms.status;
}