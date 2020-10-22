#include <stdio.h>
#include <squire.h>

void test(){
	printf("Other thread!\r\n");
	for(int i=0; i<10; i++){
		printf("B\r\n");
	}
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	void * stack = malloc(4096);
	unsigned int threadB = squire_syscall_thread(test, stack, 4096, 0);
	printf("New thread [%d]\r\n", threadB);

	squire_syscall_join(threadB);

	printf("A\r\n");
	for(;;){
	}
}
 