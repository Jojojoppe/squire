#include <squire_syscall.h>

int squire_io_port_outb(unsigned int port, unsigned char val){
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTB;
	params.value0 = port;
	params.value1 = val;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	return params.return0;
}
