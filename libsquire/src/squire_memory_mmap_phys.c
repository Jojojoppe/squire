#include <squire_syscall.h>

void * squire_memory_mmap_phys(void * addr, void * phys, size_t length, int flags){
    squire_syscall_memory_t params;
    params.operation = SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP_PHYS;
    params.flags = flags;
    params.address0 = addr;
	params.address1 = phys;
    params.length0 = length;
    squire_syscall(SQUIRE_SYSCALL_MEMORY, sizeof(params), &params);
    return params.address0;
}
