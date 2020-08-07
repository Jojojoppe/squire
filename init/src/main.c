#include <stdio.h>
#include <squire.h>

extern void * tar_getfile(void *, void *, int *);

int main(int argc, char ** argv){
	printf("This is init\r\n");
	printf("%s\r\n", argv[0]);
	unsigned int * argv1 = (unsigned int*) argv[1];
	printf("testbin at %08x\r\n", *argv1);

	/*
	// Load testbin.bin and run
	void * tarstart = (void *) argv[1];_CPU
	int testbin_length;
	void * testbin = tar_getfile("testbin.bin", (unsigned int)tarstart, &testbin_length);
	squire_syscall_process(testbin, testbin_length);
	*/
}
