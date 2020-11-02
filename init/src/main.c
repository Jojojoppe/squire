#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

void SIGINTRhandler(int sig){
	printf("SIGINTR occured: interrupt id %08x\r\n", squire_extraval0);
}

int main(int argc, char ** argv){
	signal(SIGINTR, SIGINTRhandler);
	printf("Main thread of init.bin\r\n");

	// Register ISR 40
	squire_syscall_io_t params;
	params.operation = SQUIRE_SYSCALL_IO_OPERATION_REGISTER_ISR;
	params.value0 = 40;
	squire_syscall(SQUIRE_SYSCALL_IO, sizeof(params), &params);
	printf("Registration value: %d\r\n", params.return0);

	asm("int $40");

	for(;;);
	return 0;
} 
