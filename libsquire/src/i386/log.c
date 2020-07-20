#include "../include/squire.h"

void squire_syscall_log(char * data, size_t length){
	squire_params_log_t parms;
	parms.data = data;
	parms.length = length;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_LOG),"c"(sizeof(parms)),"d"(&parms));
}
