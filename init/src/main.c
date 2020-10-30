#include <squire.h>
#include <stdlib.h>
#include <stdio.h>

#include "tar.h"

int A(void * p){

	for(;;){
		printf("A\r\n");
	}
	return 0;
}

int main(int argc, char ** argv){
	printf("This is init.bin!!!\r\n");

	void * stackA = malloc(4096);
	unsigned int threadA = squire_syscall_thread(A, stackA, 4096, 0, 0);

	for(;;){
		printf("main\r\n");
	}

	for(;;);
	return 0;
}
 
