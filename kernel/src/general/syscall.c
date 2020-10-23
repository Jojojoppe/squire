#include <general/syscall.h>
#include <general/kprintf.h>
#include <general/config.h>
#include <general/schedule.h>

#include <general/arch/proc.h>
#include <general/vmm.h>
#include <general/arch/vas.h>
#include <general/message.h>

unsigned int syscall_mmap(squire_params_mmap_t * params){
    // Check OK memory region
    if(params->length<PAGE_SIZE || params->length>VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;
    if(params->address<VMM_USERREGION_BASE || params->address+params->length > VMM_USERREGION_BASE+VMM_USERREGION_LENGTH)
        return SYSCALL_ERROR_PARAMS;

    vmm_region_t * memory = proc_get_memory();
    // printf("vmm_alloc(%08x, %08x)\r\n", params->address, params->length);
    // printf("current vas: %08x\r\n", memory);
    // printf("current proc: %08x\r\n", proc_proc_get_current());
    if(vmm_alloc(params->address, params->length, VMM_FLAGS_READ|VMM_FLAGS_WRITE, &memory)){
        // printf("ERROR\r\n");
        return SYSCALL_ERROR_PARAMS;
    }
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
    params->retval = 0;
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
                // Get thread from killed list
                t = proc_proc_get_current()->killed_threads;
                while(t->id!=params->id){
                    t = t->next;
                    if(!t){
                        // ERROR!!! THREAD NOT IN KILLED LIST
                        return 1;
                    }
                }
                params->retval = t->retval;
                break;
            }
            found = 0;
            schedule();
        }
    }
    return 0;
}

unsigned int syscall_exit(squire_params_exit_t * params){
    proc_thread_kill(proc_thread_get_current(), proc_proc_get_current(), params->retval);
    // proc_debug();
    return 0;
}

unsigned int syscall_process(squire_params_process_t * params){
    // printf("Creating process\r\n");
    // Copy ELF to kernel
    // TODO something else than vas_brk!!!
    void * newelf = vas_brk((params->elf_length/4096+1)*4096);
    memcpy(newelf, params->elf_start, params->elf_length);
    proc_proc_t * pnew = proc_proc_new(newelf);

    unsigned int params_data1[2];
    params_data1[0] = params->argc;
    params_data1[1] = params->param_data_size;
    unsigned int status = message_simple_send(pnew->id, sizeof(unsigned int)*2, params_data1);
    message_simple_send(pnew->id, params_data1[1], params->param_data);
    params->pid = pnew->id;
    // printf("Process created!\r\n");

    return 0;
}


unsigned int syscall_simple_send(squire_params_simple_send_t * params){
    unsigned int status = message_simple_send(params->to, params->length, params->data);
    params->status = status;
    return status;
}

unsigned int syscall_simple_recv(squire_params_simple_recv_t * params){
    unsigned int status = 0;
    size_t length = params->length;
    if(params->blocked)
        status = message_simple_receive_blocking(params->buffer, &length, &params->from);
    else
        status = message_simple_receive(params->buffer, &length, &params->from);
    params->status = status;
    params->length = length;
    return status;
}


unsigned int syscall_log(squire_params_log_t * params){
    for(int i=0; i<params->length; i++)
        printf("%c", params->data[i]);
    return 0;
}

unsigned int syscall(unsigned int opcode, void * param_block, size_t param_len){
    unsigned int returncode;
    // printf("SYSCALL %08x\r\n", opcode);

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

        case SQUIRE_SYSCALL_EXIT:{
            if(param_len<sizeof(squire_params_exit_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_exit_t * params = (squire_params_exit_t*)param_block;
            returncode = syscall_exit(params);
        } break;

        case SQUIRE_SYSCALL_PROCESS:{
            if(param_len<sizeof(squire_params_process_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_process_t * params = (squire_params_process_t*)param_block;
            returncode = syscall_process(params);
        } break;



        case SQUIRE_SYSCALL_SIMPLE_SEND:{
            if(param_len<sizeof(squire_params_simple_send_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_simple_send_t * params = (squire_params_simple_send_t*)param_block;
            returncode = syscall_simple_send(params);
        } break;

        case SQUIRE_SYSCALL_SIMPLE_RECV:{
            if(param_len<sizeof(squire_params_simple_recv_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_simple_recv_t * params = (squire_params_simple_recv_t*)param_block;
            returncode = syscall_simple_recv(params);
        } break;


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