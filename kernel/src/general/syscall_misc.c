#include <general/syscall_misc.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/timer.h>
#include <general/arch/proc.h>

int syscall_misc(squire_syscall_misc_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_MISC_OPERATION_TIMER_ADD:{
            timer_add_callback(proc_proc_get_current()->id, params->longvalue0, params->value0);
        }break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}