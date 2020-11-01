#include <squire_syscall.h>

squire_mutex_t squire_mutex_create(){
    squire_syscall_mutex_t params;
    params.operation = SQUIRE_SYSCALL_MUTEX_OPERATION_CREATE;
    squire_syscall(SQUIRE_SYSCALL_MUTEX, sizeof(params), &params);
    return params.mtx0;
}