#include <stdio.h>
#include <squire.h>

int test(void * p){
	printf("p = %08x\r\n", p);
	return (int)p * 2;
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");
	printf("argc: %08x argv: %08x\r\n", argc, argv);

	void * stack = malloc(4096);
	unsigned int threadB = squire_syscall_thread(test, stack, 4096, 0, 0x11223344);
	printf("New thread [%d]\r\n", threadB);

	int retB = squire_syscall_join(threadB);
	printf("retB = %08x\r\n", retB);

	return 0;
}
 