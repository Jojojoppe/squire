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
	squire_syscall_thread(test, stack, 4096, 0);

	for(;;){
	}
}
