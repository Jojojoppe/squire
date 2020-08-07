#include <stdio.h>
#include <squire.h>

extern void * tar_getfile(void *, void *, int *);

int main(int argc, char ** argv){
	printf("This is init\r\n");
	unsigned int * argv1 = (unsigned int*) argv[1];

	// Load testbin.bin and run
	void * tarstart = (void *) *argv1;
	printf("initramfs at %08x\r\n", tarstart);
	int testbin_length;
	void * testbin = tar_getfile("testbin.bin", (unsigned int)tarstart, &testbin_length);
	char * argv_testbin[2] = {"testbin", "abcde"};
	squire_syscall_process(testbin, testbin_length, 2, &argv_testbin);
}
