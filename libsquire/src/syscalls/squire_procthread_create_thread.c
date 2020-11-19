#include <squire_syscall.h>

unsigned int squire_procthread_create_thread(int (*entry)(void*), void * stack_base, size_t stack_length, int flags, void * param){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_THREAD;
    params.func0 = entry;
    params.address0 = stack_base;
    params.length0 = stack_length;
    params.address1 = param;
    params.flags = flags;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
    return params.tid0;
}