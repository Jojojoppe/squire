#include <squire.h>
#include <stdlib.h>
#include <stdio.h>

#include "tar.h"

squire_mutex_t mutex;

int A(void * p){

	for(;;){
		squire_syscall_mutex_lock(mutex);
		for(int i=0; i<10000000; i++);
		printf("A\r\n");
		squire_syscall_mutex_unlock(mutex);
		for(int i=0; i<10000000; i++);
	}
	return 0;
}

int main(int argc, char ** argv){
	printf("This is init.bin!!!\r\n");

	mutex = squire_syscall_mutex_init();

	void * stackA = malloc(4096);
	unsigned int threadA = squire_syscall_thread(A, stackA, 4096, 0, 0);

	for(;;){
		squire_syscall_mutex_lock(mutex);
		for(int i=0; i<10000000; i++);
		squire_syscall_log("main\r\n", 6);
		squire_syscall_mutex_unlock(mutex);
		for(int i=0; i<10000000; i++);
	}

	for(;;);
	return 0;
}
 
