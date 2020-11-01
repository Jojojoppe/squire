#include <squire_syscall.h>

void squire_procthread_exit_thread(int returnvalue){
    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_EXIT_THREAD;
    params.value0 = returnvalue;
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
}