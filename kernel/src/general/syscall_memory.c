#include <general/syscall_memory.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/config.h>
#include <general/arch/proc.h>
#include <general/vmm.h>

int syscall_memory(squire_syscall_memory_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP:{
            // Check OK memory region
            if(params->length0<PAGE_SIZE || params->length0>VMM_USERREGION_LENGTH)
                return SYSCALL_ERROR_PARAMS;
            if(params->address0!=0 && (params->address0<VMM_USERREGION_BASE || params->address0+params->length0 > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH))
                return SYSCALL_ERROR_PARAMS;

            unsigned int flags;
            if((params->flags&MMAP_EXEC))
                flags |= VMM_FLAGS_EXEC;
            if((params->flags&MMAP_READ))
                flags |= VMM_FLAGS_READ;
            if((params->flags&MMAP_WRITE))
                flags |= VMM_FLAGS_WRITE;

            vmm_region_t * memory = proc_get_memory();
			// If automatic allocation
			if(params->address0){
				if(vmm_alloc(params->address0, params->length0, flags, &memory)){
					params->length0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}else{
				if(vmm_alloc_auto(&params->address0, params->length0, flags, &memory)){
					params->length0 = 0;
					params->address0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}
            proc_set_memory(memory);
            
        }break;

        case SQUIRE_SYSCALL_MEMORY_OPERATION_MMAP_PHYS:{
			// Check OK memory region
            if(params->length0<PAGE_SIZE || params->length0>VMM_USERREGION_LENGTH)
                return SYSCALL_ERROR_PARAMS;
            if(params->address0!=0 && (params->address0<VMM_USERREGION_BASE || params->address0+params->length0 > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH))
                return SYSCALL_ERROR_PARAMS;

            unsigned int flags = 0;
            if((params->flags&MMAP_EXEC))
                flags |= VMM_FLAGS_EXEC;
            if((params->flags&MMAP_READ))
                flags |= VMM_FLAGS_READ;
            if((params->flags&MMAP_WRITE))
                flags |= VMM_FLAGS_WRITE;

            vmm_region_t * memory = proc_get_memory();
			// If automatic allocation
			if(params->address0){
				if(vmm_map_phys(params->address0, params->address1, params->length0, flags, &memory)){
					params->length0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}else{
				if(vmm_map_phys_auto(&params->address0, params->address1, params->length0, flags, &memory)){
					params->length0 = 0;
					params->address0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}
            proc_set_memory(memory);
        }break;

        case SQUIRE_SYSCALL_MEMORY_OPERATION_CREATE_SHARED:{
            // Check OK memory region
            if(params->length0<PAGE_SIZE || params->length0>VMM_USERREGION_LENGTH)
                return SYSCALL_ERROR_PARAMS;
            if(params->address0!=0 && (params->address0<VMM_USERREGION_BASE || params->address0+params->length0 > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH))
                return SYSCALL_ERROR_PARAMS;

            unsigned int flags;
            if((params->flags&MMAP_EXEC))
                flags |= VMM_FLAGS_EXEC;
            if((params->flags&MMAP_READ))
                flags |= VMM_FLAGS_READ;
            if((params->flags&MMAP_WRITE))
                flags |= VMM_FLAGS_WRITE;

            vmm_region_t * memory = proc_get_memory();
			// If automatic allocation
			if(params->address0){
				if(vmm_create_shared(params->address0, params->length0, flags, params->id0, &memory)){
					params->length0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}else{
				if(vmm_create_shared_auto(&params->address0, params->length0, flags, params->id0, &memory)){
					params->length0 = 0;
					params->address0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}
            proc_set_memory(memory);
            
        }break;

        case SQUIRE_SYSCALL_MEMORY_OPERATION_MAP_SHARED:{
            unsigned int flags;
            if((params->flags&MMAP_EXEC))
                flags |= VMM_FLAGS_EXEC;
            if((params->flags&MMAP_READ))
                flags |= VMM_FLAGS_READ;
            if((params->flags&MMAP_WRITE))
                flags |= VMM_FLAGS_WRITE;

            vmm_region_t * memory = proc_get_memory();
			// If automatic allocation
			if(params->address0){
				if(vmm_map_shared(params->address0, flags, params->pid0, params->id0, &memory)){
					params->length0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}else{
				if(vmm_map_shared_auto(&params->address0, flags, params->pid0, params->id0, &memory)){
					params->length0 = 0;
					params->address0 = 0;
					return SYSCALL_ERROR_GENERAL;
				}
			}
            proc_set_memory(memory);
            
        }break;


        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}
