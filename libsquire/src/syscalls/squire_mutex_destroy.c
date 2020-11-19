#include <squire_syscall.h>

void squire_mutex_destroy(squire_mutex_t mtx){
    squire_syscall_mutex_t params;
    params.operation = SQUIRE_SYSCALL_MUTEX_OPERATION_DESTROY;
    params.mtx0 = mtx;
    squire_syscall(SQUIRE_SYSCALL_MUTEX, sizeof(params), &params);
}