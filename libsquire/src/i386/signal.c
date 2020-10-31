#include "../include/squire.h"

int _signal_handler(void * p){
    squire_params_signal_t * parms = (squire_params_signal_t*)p;
    while(1){
        unsigned int sig = squire_syscall_signal_get();
        parms->handler(sig);
    }
    free(p);
    return 0;
}

void squire_syscall_signal(int (*handler)(int)){
    squire_params_signal_t * parms = (squire_params_signal_t*)malloc(sizeof(squire_params_signal_t));
    parms->handler = handler;
    parms->stack = malloc(4096);
    parms->stack_length = 4096;
    parms->tid = squire_syscall_thread(_signal_handler, parms->stack, 4096, SQUIRE_THREAD_PRIORITY, parms);

	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_SIGNAL),"c"(sizeof(squire_params_signal_t)),"d"(parms));
}

unsigned int squire_syscall_signal_get(){
    squire_params_signal_get_t parms;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_SIGNAL_GET),"c"(sizeof(parms)),"d"(&parms));
    return parms.value;
}