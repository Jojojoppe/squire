#include <squire_syscall.h>

void * squire_memory_mmap(void * addr, size_t length, int flags){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP;
    params.flags = flags;
    params.address0 = addr;
    params.length0 = length;
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
    return params.address0;
}