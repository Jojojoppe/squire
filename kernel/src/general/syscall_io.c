#include <general/syscall_io.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/user_io.h>
#include <general/arch/proc.h>

int syscall_io(squire_syscall_io_t * params){
    switch(params->operation){

		case SQUIRE_SYSCALL_IO_OPERATION_REGISTER_ISR:{
			unsigned int pid = proc_proc_get_current()->id;
			params->return0 = user_io_register_isr(params->value0, pid);
	    } break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}
