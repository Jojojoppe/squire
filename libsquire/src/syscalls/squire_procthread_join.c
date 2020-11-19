#include <squire_syscall.h>

int squire_procthread_join(unsigned int tid){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_JOIN_THREAD;
    params.tid0 = tid;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
    return params.value0;
}