#include <stdio.h>
#include <squire.h>

#include "tar.h"

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

	for(int i=0; i<500000000; i++);
	char * test0 = "This is a test message which will be sent to testbin.bin";
	length = strlen(test0);
	squire_syscall_simple_send(testbin_pid, length, test0);

	for(int i=0; i<500000000; i++);
	char * test1 = "This is another test message which will be sent to testbin.bin";
	length = strlen(test1);
	squire_syscall_simple_send(testbin_pid, length, test1);

	for(;;){
	}

	return 0;
}
 