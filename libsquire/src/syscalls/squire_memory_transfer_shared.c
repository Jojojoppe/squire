#include <squire_syscall.h>

void squire_memory_transfer_shared(char id[32]){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_TRANSFER_SHARED;
	memcpy(params.id0, id, 32);
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
}
