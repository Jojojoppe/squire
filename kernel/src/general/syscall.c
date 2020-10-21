#include <general/syscall.h>
#include <general/kprintf.h>
#include <general/config.h>

#include <general/arch/proc.h>
#include <general/vmm.h>

unsigned int syscall_mmap(squire_params_mmap_t * params){
    printf("vmm_alloc(%08x, %08x)\r\n", params->address, params->length);
    // Check OK memory region
    if(params->length<PAGE_SIZE || params->length>VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;
    if(params->address<VMM_USERREGION_BASE || params->address+params->length > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;

    vmm_region_t * memory = proc_get_memory();
    if(vmm_alloc(params->address, params->length, VMM_FLAGS_READ|VMM_FLAGS_WRITE, memory))
        return SYSCALL_ERROR_PARAMS;

    return 0;
}

unsigned int syscall_log(squire_params_log_t * params){
    for(int i=0; i<params->length; i++)
        printf("%c", params->data[i]);
    return 0;
}

unsigned int syscall(unsigned int opcode, void * param_block, size_t param_len){
    printf("SYSCALL [%08x] - %08x %d\r\n", opcode, param_block, param_len);
    switch(opcode){

        case SQUIRE_SYSCALL_MMAP:{
            if(param_len<sizeof(squire_params_mmap_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mmap_t * params = (squire_params_mmap_t*)param_block;
            return syscall_mmap(params);
        } break;

        case SQUIRE_SYSCALL_LOG:{
            if(param_len<sizeof(squire_params_log_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_log_t * params = (squire_params_log_t*)param_block;
            return syscall_log(params);
        } break;

    }

    return SYSCALL_ERROR_GENERAL;
}