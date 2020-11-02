#include <squire_syscall.h>

int squire_io_port_inb(unsigned int port, unsigned char * val){
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_PORT_INB;
	params.value0 = port;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	*val = params.value1;
	return params.return0;
}
