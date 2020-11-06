#include <general/syscall_debug.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/schedule.h>
#include <general/arch/proc.h>
#include <general/wait.h>
#include <general/kill.h>
#include <general/arch/vas.h>
#include <general/string.h>
#include <general/kmalloc.h>

int syscall_procthread(squire_syscall_procthread_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_THREAD:{
            schedule_disable();
            proc_proc_t * pcur = proc_proc_get_current();
            proc_thread_t * tnew = proc_thread_new_user(params->func0, params->address0, params->length0, pcur);
            params->tid0 = tnew->id;
            if((params->flags&THREAD_QUEUE_PRIORITY)){
                schedule_move_queue(schedule_get(pcur->id, tnew->id), SCHEDULE_QUEUE_TYPE_PRIORITY);
            }
            unsigned int * s = (unsigned int*)(params->address0+params->length0);
            *(s) = (unsigned int)params->address1;
            schedule_enable();
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_EXIT_THREAD:{
            schedule_kill(0, params->value0);
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_EXIT_PROCESS:{
            exit(params->value0);
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GET_IDS:{
            params->tid0 = proc_thread_get_current()->id;
            params->pid0 = proc_proc_get_current()->id;
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_WAIT_PROCESS: {
            params->value0 = wait(&params->value1, params->pid0);
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_JOIN_THREAD:{
            proc_proc_t * pcur = proc_proc_get_current();
            unsigned int pid = pcur->id;
            params->value0 = -1;
            while(1){
                schedule_schedulable_t * s = schedule_get(pid, params->tid0);
                if(s){
                    // Still scheduled
                    // TODO set thread idle
                    schedule();
                }else{
                    // Not scheduled (or non existing)
                    proc_thread_t * kt = pcur->killed_threads;
                    while(kt){
                        if(kt->id == params->tid0){
                            params->value0 = kt->retval;
                            // TODO remove thread from killed thread list
                            break;
                        }
                        kt = kt->next;
                    }
                    // Thread non existing
                    break;
                }
            }
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_KILL:{
            kill(params->pid0, params->value0);
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_SIGNAL:{
            proc_proc_get_current()->signal_handler = params->tid0;
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GETSIG:{
            proc_proc_t * pcur = proc_proc_get_current();
            while(!pcur->signals){
                schedule_set_state(0, SCHEDULE_STATE_IDLE);
                schedule();
            }
            signal_t * s = pcur->signals;
            pcur->signals = s->next;
            params->value0 = s->value;
            params->value1 = s->extraval0;
            params->value2 = s->extraval1;
            params->value3 = s->extraval2;
            params->value4 = s->extraval3;
            kfree(s);
        } break;

        case SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_PROCESS:{
            // TODO use something else than brk!!!
            //void * newelf = vas_brk((params->length0/4096+1)*4096);
			void * newelf = kmalloc((params->length0/4096+1)*4096);
            memcpy(newelf, params->address0, params->length0);
            proc_proc_t * pnew = proc_proc_new(newelf);
            unsigned int params_data1[2];
            params_data1[0] = params->value0;
            params_data1[1] = params->length1;
            unsigned int status = message_simple_send(pnew->id, sizeof(unsigned int)*2, params_data1);
            status = message_simple_send(pnew->id, params_data1[1], params->address1);
            params->pid0 = pnew->id;
			kfree(newelf);
        } break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}
