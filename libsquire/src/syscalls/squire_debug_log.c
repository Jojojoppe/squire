#include <squire_syscall.h>

size_t squire_debug_log(const char * message, size_t length){
    
    squire_syscall_debug_t params;
    params.operation = SQUIRE_SYSCALL_DEBUG_OPERATION_LOG;
    params.data0 = message;
    params.length0 = length;
    squire_syscall(SQUIRE_SYSCALL_DEBUG, sizeof(params), &params);

    return length;
}