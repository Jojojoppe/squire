#include <general/syscall.h>
#include <general/kprintf.h>
#include <general/config.h>
#include <general/schedule.h>

#include <general/arch/proc.h>
#include <general/vmm.h>

unsigned int syscall_mmap(squire_params_mmap_t * params){
    // Check OK memory region
    if(params->length<PAGE_SIZE || params->length>VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;
    if(params->address<VMM_USERREGION_BASE || params->address+params->length > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;

    vmm_region_t * memory = proc_get_memory();
    if(vmm_alloc(params->address, params->length, VMM_FLAGS_READ|VMM_FLAGS_WRITE, &memory))
        return SYSCALL_ERROR_PARAMS;
    proc_set_memory(memory);

    return 0;
}

unsigned int syscall_thread(squire_params_thread_t * params){
    schedule_disable();
        proc_thread_t * new = proc_thread_new_user(params->entry, params->stack_base, params->stack_length, proc_proc_get_current());
        params->entry = new->id;
        proc_proc_t * pcurrent = proc_proc_get_current();
        params->stack_base = pcurrent->id;
    schedule_enable();
    // printf("New thread created\r\n");
    // asm("int $0");
    return 0;
}

unsigned int syscall_join(squire_params_join_t * params){
    // Find thread with TID
    proc_thread_t * t = proc_proc_get_current()->threads;
    unsigned char found = 0;
    while(t){
        if(t->id == params->id){
            found = 1;
        }
        t = t->next;
        if(!t){
            t = proc_proc_get_current()->threads;
            if(!found){
                break;
            }
            found = 0;
            schedule();
        }
    }
    return 0;
}

unsigned int syscall_process(squire_params_process_t * params){
    // asm("int $0");
    return 0;
}

unsigned int syscall_log(squire_params_log_t * params){
    for(int i=0; i<params->length; i++)
        printf("%c", params->data[i]);
    return 0;
}

unsigned int syscall(unsigned int opcode, void * param_block, size_t param_len){
    unsigned int returncode;

    switch(opcode){

        case SQUIRE_SYSCALL_MMAP:{
            if(param_len<sizeof(squire_params_mmap_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mmap_t * params = (squire_params_mmap_t*)param_block;
            returncode =  syscall_mmap(params);
        } break;

        case SQUIRE_SYSCALL_THREAD:{
            if(param_len<sizeof(squire_params_thread_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_thread_t * params = (squire_params_thread_t*)param_block;
            returncode = syscall_thread(params);
        } break;

        case SQUIRE_SYSCALL_JOIN:{
            if(param_len<sizeof(squire_params_join_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_join_t * params = (squire_params_join_t*)param_block;
            returncode = syscall_join(params);
        } break;

        case SQUIRE_SYSCALL_PROCESS:{
            if(param_len<sizeof(squire_params_process_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_process_t * params = (squire_params_process_t*)param_block;
            returncode = syscall_process(params);
        };

        case SQUIRE_SYSCALL_LOG:{
            if(param_len<sizeof(squire_params_log_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_log_t * params = (squire_params_log_t*)param_block;
            returncode = syscall_log(params);
        } break;

        default:
            return SYSCALL_ERROR_GENERAL;

    }

    schedule();

    return returncode;
}