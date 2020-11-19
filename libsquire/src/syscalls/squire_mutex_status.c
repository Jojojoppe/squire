#include <squire_syscall.h>

int squire_mutex_status(squire_mutex_t mtx){
    squire_syscall_mutex_t params;
    params.operation = SQUIRE_SYSCALL_MUTEX_OPERATION_STATUS;
    params.mtx0 = mtx;
    squire_syscall(SQUIRE_SYSCALL_MUTEX, sizeof(params), &params);
    return params.value0;
}