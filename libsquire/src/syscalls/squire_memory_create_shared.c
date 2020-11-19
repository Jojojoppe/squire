#include <squire_syscall.h>

void * squire_memory_create_shared(void * addr, size_t length, char id[32], int flags){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_CREATE_SHARED;
    params.flags = flags;
    params.address0 = addr;
    params.length0 = length;
	memcpy(params.id0, id, 32);
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
    return params.address0;
}
