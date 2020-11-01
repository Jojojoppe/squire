#include <squire_syscall.h>

void squire_procthread_kill(unsigned int pid, int signal){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_KILL;
    params.pid0 = pid;
    params.value0 = signal;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
}