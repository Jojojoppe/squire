#include <squire_syscall.h>

void * squire_memory_map_shared(void * addr, unsigned int pid, char id[32], int flags){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_MAP_SHARED;
    params.flags = flags;
    params.address0 = addr;
	params.pid0 = pid;
	memcpy(params.id0, id, 32);
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
    return params.address0;
}
