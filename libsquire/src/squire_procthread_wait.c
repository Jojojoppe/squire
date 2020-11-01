#include <squire_syscall.h>

int squire_procthread_wait(unsigned int pid, int * returnvalue){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_WAIT_PROCESS;
    params.pid0 = pid;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
    if(returnvalue)
        *returnvalue = params.value1;
    return params.value0;
}