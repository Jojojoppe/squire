#include <squire_syscall.h>

int squire_io_register_isr(unsigned int id){
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_REGISTER_ISR;
	params.value0 = id;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	return params.return0;
}
