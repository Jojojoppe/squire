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

		case SQUIRE_SYSCALL_IO_OPERATION_REGISTER_PORT:{
			unsigned int pid = proc_proc_get_current()->id;
			params->return0 = user_io_register_port(params->value0, params->value1, params->flags, pid);
		} break;

		case SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTB:{
			params->return0 = user_io_port_outb(params->value0, params->value1);
	    } break;
		case SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTW:{
			params->return0 = user_io_port_outw(params->value0, params->value1);
	    } break;
		case SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTD:{
			params->return0 = user_io_port_outd(params->value0, params->value1);
	    } break;

		case SQUIRE_SYSCALL_IO_OPERATION_PORT_INB:{
			params->return0 = user_io_port_inb(params->value0, &params->value1);
	    } break;
		case SQUIRE_SYSCALL_IO_OPERATION_PORT_INW:{
			params->return0 = user_io_port_inw(params->value0, &params->value1);
	    } break;
		case SQUIRE_SYSCALL_IO_OPERATION_PORT_IND:{
			params->return0 = user_io_port_ind(params->value0, &params->value1);
	    } break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}
