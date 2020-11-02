#include <squire_syscall.h>

int squire_io_register_port(unsigned int port, unsigned int range, unsigned int flags){
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_REGISTER_PORT;
	params.value0 = port;
	params.value1 = range;
	params.flags = flags;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	return params.return0;
}
