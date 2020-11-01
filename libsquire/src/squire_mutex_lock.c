#include <squire_syscall.h>

void squire_mutex_lock(squire_mutex_t mtx){
    squire_syscall_mutex_t params;
    params.operation = SQUIRE_SYSCALL_MUTEX_OPERATION_LOCK;
    params.mtx0 = mtx;
    squire_syscall(SQUIRE_SYSCALL_MUTEX, sizeof(params), &params);
}