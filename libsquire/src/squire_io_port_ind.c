#include <squire_syscall.h>

int squire_io_port_ind(unsigned int port, unsigned int * val){
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_PORT_IND;
	params.value0 = port;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	*val = params.value1;
	return params.return0;
}
