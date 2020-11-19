#include <squire_syscall.h>

void squire_memory_munmap(void * addr, size_t length){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_MUNMAP;
    params.address0 = addr;
    params.length0 = length;
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
}
