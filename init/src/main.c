#include <stdio.h>
#include <squire.h>

void test2(){
	for(int j=0; j<10; j++){
		printf("C\r\n");
	}
	squire_syscall_exit(0xaabbccdd);
}

void test(){
	for(int j=0; j<10; j++){
		printf("B\r\n");
	}
	squire_syscall_exit(0x00ff1122);
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	void * stack = malloc(4096);
	unsigned int threadB = squire_syscall_thread(test, stack, 4096, 0);
	printf("New thread [%d]\r\n", threadB);

	void * stack2 = malloc(4096);
	unsigned int threadC = squire_syscall_thread(test2, stack2, 4096, 0);
	printf("New thread [%d]\r\n", threadC);

	int retB = squire_syscall_join(threadB);
	printf("retB = %08x\r\n", retB);
	int retC = squire_syscall_join(threadC);
	printf("retC = %08x\r\n", retC);

	for(;;);

	return 0;
}
 