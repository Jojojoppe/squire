#include <general/syscall.h>
#include <general/kprintf.h>
#include <general/config.h>
#include <general/schedule.h>

#include <general/arch/proc.h>
#include <general/vmm.h>
#include <general/arch/vas.h>
#include <general/message.h>
#include <general/mutex.h>
#include <general/wait.h>
#include <general/kill.h>

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

        // Move to priority queue if flags bit 1 is set
        if((params->flags&1)==1){
            schedule_move_queue(schedule_get(pcurrent->id, new->id), SCHEDULE_QUEUE_TYPE_PRIORITY);
        }

    schedule_enable();
    // printf("New thread created\r\n");
    // asm("int $0");
    return 0;
}

unsigned int syscall_join(squire_params_join_t * params){
    proc_proc_t * process = proc_proc_get_current();
    unsigned int pid = process->id;
    params->retval = 0;
    while(1){
        schedule_schedulable_t * s = schedule_get(pid, params->id);
        if(s){
            // Still scheduled
            // TODO set thread idle
            schedule();
        }else{
            // Not scheduled anymore!
            // Try to find in killed thread list
            proc_thread_t * kt = process->killed_threads;
            while(kt){
                if(kt->id == params->id){
                    params->retval = kt->retval;
                    return 0;
                }
                kt = kt->next;
            }
            // If reached here, thread is not scheduled and not in killed list: error
            break;
        }        
    }
    return 1;
}

unsigned int syscall_exit(squire_params_exit_t * params){
    exit(params->retval);
    return 0;
}

unsigned int syscall_thread_exit(squire_params_exit_t * params){
    schedule_kill(0, params->retval);
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
    status = message_simple_send(pnew->id, params_data1[1], params->param_data);
    params->pid = pnew->id;
    // printf("Process created!\r\n");

    return 0;
}

unsigned int syscall_wait(squire_params_wait_t * params){
    params->reason = wait(&params->retval, params->pid);
    return 0;
}

unsigned int syscall_kill(squire_params_kill_t * params){
    kill(params->pid, params->reason);
    return 0;
}

unsigned int syscall_getid(squire_params_getid_t * params){
    params->pid = proc_proc_get_current()->id;
    params->tid = proc_thread_get_current()->id;
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


unsigned int syscall_mutex_init(squire_params_mutex_t * params){
	params->mutex = (void*) mutex_create();
	return 0;
}

unsigned int syscall_mutex_deinit(squire_params_mutex_t * params){
	mutex_destroy((mutex_t*)params->mutex);
	params->mutex = 0;
	return 0;
}

unsigned int syscall_mutex_lock(squire_params_mutex_t * params){
	mutex_lock((mutex_t*)params->mutex);
	return 0;
}

unsigned int syscall_mutex_unlock(squire_params_mutex_t * params){
	mutex_unlock((mutex_t*)params->mutex);
	return 0;
}

unsigned int syscall_mutex_status(squire_params_mutex_status_t * params){
	params->status = mutex_get((mutex_t*)params->mutex);
	return 0;
}


unsigned int syscall_signal(squire_params_signal_t * params){
    proc_proc_get_current()->signal_handler = params->tid;
    return 0;
}

unsigned int syscall_signal_get(squire_params_signal_get_t * params){
    proc_proc_t * pcur = proc_proc_get_current();
    while(!pcur->signals){
        schedule_set_state(0, SCHEDULE_STATE_IDLE);
        schedule();
    }
    signal_t * s = pcur->signals;
    pcur->signals = s->next;
    params->value = s->value;
    kfree(s);
    return 0;
}

unsigned int syscall_log(squire_params_log_t * params){
    printf("[%3d,%3d] ", proc_proc_get_current()->id, proc_thread_get_current()->id);
    for(int i=0; i<params->length; i++)
        printf("%c", params->data[i]);
    return 0;
}

unsigned int syscall(unsigned int opcode, void * param_block, size_t param_len){
    unsigned int returncode;
    // printf("[%3d,%3d] SYSCALL %08x\r\n", proc_proc_get_current()->id, proc_thread_get_current()->id, opcode);

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

        case SQUIRE_SYSCALL_THREAD_EXIT:{
            if(param_len<sizeof(squire_params_exit_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_exit_t * params = (squire_params_exit_t*)param_block;
            returncode = syscall_thread_exit(params);
        } break;

        case SQUIRE_SYSCALL_PROCESS:{
            if(param_len<sizeof(squire_params_process_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_process_t * params = (squire_params_process_t*)param_block;
            returncode = syscall_process(params);
        } break;

		case SQUIRE_SYSCALL_YIELD:{
			schedule();
			returncode = 0;				  
		} break;

        case SQUIRE_SYSCALL_WAIT:{
            if(param_len<sizeof(squire_params_wait_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_wait_t * params = (squire_params_wait_t*)param_block;
            returncode = syscall_wait(params);
        } break;

        case SQUIRE_SYSCALL_KILL:{
            if(param_len<sizeof(squire_params_kill_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_kill_t * params = (squire_params_kill_t*)param_block;
            returncode = syscall_kill(params);
        } break;

        case SQUIRE_SYSCALL_GETID:{
            if(param_len<sizeof(squire_params_getid_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_getid_t * params = (squire_params_getid_t*)param_block;
            returncode = syscall_getid(params);
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



        case SQUIRE_SYSCALL_MUTEX_INIT:{
            if(param_len<sizeof(squire_params_mutex_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mutex_t * params = (squire_params_mutex_t*)param_block;
            returncode = syscall_mutex_init(params);
        } break;

        case SQUIRE_SYSCALL_MUTEX_DEINIT:{
            if(param_len<sizeof(squire_params_mutex_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mutex_t * params = (squire_params_mutex_t*)param_block;
            returncode = syscall_mutex_deinit(params);
        } break;

        case SQUIRE_SYSCALL_MUTEX_LOCK:{
            if(param_len<sizeof(squire_params_mutex_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mutex_t * params = (squire_params_mutex_t*)param_block;
            returncode = syscall_mutex_lock(params);
        } break;

        case SQUIRE_SYSCALL_MUTEX_UNLOCK:{
            if(param_len<sizeof(squire_params_mutex_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mutex_t * params = (squire_params_mutex_t*)param_block;
            returncode = syscall_mutex_unlock(params);
        } break;

        case SQUIRE_SYSCALL_MUTEX_STATUS:{
            if(param_len<sizeof(squire_params_mutex_status_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_mutex_status_t * params = (squire_params_mutex_status_t*)param_block;
            returncode = syscall_mutex_status(params);
        } break;



        case SQUIRE_SYSCALL_SIGNAL:{
            if(param_len<sizeof(squire_params_signal_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_signal_t * params = (squire_params_signal_t*)param_block;
            returncode = syscall_signal(params);
        } break;

        case SQUIRE_SYSCALL_SIGNAL_GET:{
            if(param_len<sizeof(squire_params_signal_get_t))
                return SYSCALL_ERROR_GENERAL;
            squire_params_signal_get_t * params = (squire_params_signal_get_t*)param_block;
            returncode = syscall_signal_get(params);
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

    // schedule();
    return returncode;
}

