#include "../include/squire.h"

squire_mutex_t squire_syscall_mutex_init(){
	squire_params_mutex_t parms;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_MUTEX_INIT),"c"(sizeof(parms)),"d"(&parms));
	return parms.mutex;
}

void squire_syscall_mutex_deinit(squire_mutex_t mutex){
	squire_params_mutex_t parms;
	parms.mutex = mutex;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_MUTEX_DEINIT),"c"(sizeof(parms)),"d"(&parms));
}

void squire_syscall_mutex_lock(squire_mutex_t mutex){
	squire_params_mutex_t parms;
	parms.mutex = mutex;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_MUTEX_LOCK),"c"(sizeof(parms)),"d"(&parms));
}

void squire_syscall_mutex_unlock(squire_mutex_t mutex){
	squire_params_mutex_t parms;
	parms.mutex = mutex;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_MUTEX_UNLOCK),"c"(sizeof(parms)),"d"(&parms));
}
