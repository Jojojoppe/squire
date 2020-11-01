#include <general/syscall_debug.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/arch/proc.h>

int syscall_debug(squire_syscall_debug_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_DEBUG_OPERATION_LOG:{
            printf("[%3d,%3d] ", proc_proc_get_current()->id, proc_thread_get_current()->id);
            for(int i=0; i<params->length0; i++)
                printf("%c", params->data0[i]);
        }break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}