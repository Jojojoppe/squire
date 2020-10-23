#include "../include/squire.h"

unsigned int squire_syscall_simple_send(unsigned int to, size_t length, void * data){
    squire_params_simple_send_t parms;
    parms.to = to;
    parms.length = length;
    parms.data = data;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_SIMPLE_SEND),"c"(sizeof(parms)),"d"(&parms));
    return parms.status;
}