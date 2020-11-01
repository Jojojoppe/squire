#include <squire_syscall.h>

unsigned int squire_procthread_gettid(){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GET_IDS;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
    return params.tid0;
}