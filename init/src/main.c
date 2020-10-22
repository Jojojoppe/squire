#include <stdio.h>
#include <squire.h>

void test(){
	printf("Other thread!\r\n");
	for(;;){
	}
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	void * stack = malloc(4096)+4096-4;
	squire_syscall_thread(test, stack, 0);

	for(;;){
	}
}
