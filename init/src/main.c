#include <squire.h>
#include <stdlib.h>
#include <stdio.h>

#include "tar.h"

squire_spinlock_t lockA;

int A(void * p){

	for(;;){
		squire_spinlock_lock(&lockA);
		squire_syscall_log("A\r\n", 3);
		squire_spinlock_unlock(&lockA);
	}

	return 0;
}

int main(int argc, char ** argv){
	printf("This is init.bin!!!\r\n");

	squire_spinlock_init(&lockA);

	void * stackA = malloc(4096);
	unsigned int threadA = squire_syscall_thread(A, stackA, 4096, 0, 0);

	for(;;){
		squire_spinlock_lock(&lockA);
		for(int i=0; i<10000000; i++);
		squire_syscall_log("main\r\n", 6);
		squire_spinlock_unlock(&lockA);
		for(int i=0; i<10000000; i++);
	}

	for(;;);
	return 0;
}
 
