#include <squire.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

#include "tar.h"

int killthread(void * p){
	unsigned int pid = (unsigned int)p;
	printf("Wait to kill %d\r\n", pid);

	for(int i=0; i<100000000; i++);
	printf("KILL\r\n");
	squire_syscall_kill(2);

	return 0;
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	printf("initramfs content:\r\n");
	tar_print_list(tar_start);

	unsigned int length;
	void * testbin = tar_get(tar_start, "testbin.bin", &length);
	printf("testbin.bin starts at %08x /w length %08x\r\n", testbin, length);

	char * argv0 = "testbin.bin";
	char * argv1 = "test123";
	char ** testbin_argv[2];
	testbin_argv[0] = argv0;
	testbin_argv[1] = argv1;
	unsigned int testbin_pid = squire_syscall_process(testbin, length, 2, testbin_argv);
	printf("Testbin PID = %d\r\n", testbin_pid);

	thrd_t t_killthread;
	thrd_create(&t_killthread, killthread, testbin_pid);

	unsigned int retval;
	unsigned int reason = squire_syscall_wait(&retval, testbin_pid);
	printf("Testbin has exited with reason %d and retval %08x\r\n", reason, retval);

	for(;;);

	return 0;
} 
