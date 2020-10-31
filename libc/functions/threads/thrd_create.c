#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

struct _thrd_arg{
    thrd_start_t func;
    void * arg;
    void * stack;
};

int _thrd_start(struct _thrd_arg * param){
    int retval = param->func(param->arg);
    free(param->stack);
    free(param);
    return retval;
}

int thrd_create(thrd_t * thr, thrd_start_t func, void * arg){
    struct _thrd_arg * param = malloc(sizeof(struct _thrd_arg));
    param->arg = arg;
    param->func = func;
    param->stack = malloc(THRD_STACK_LENGTH);    
    *thr = squire_syscall_thread(_thrd_start, param->stack, THRD_STACK_LENGTH, 0, param);
    return thrd_success;
}