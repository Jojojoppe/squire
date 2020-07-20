#include <stdio.h>
#include <squire.h>

extern void * tar_getfile(void *, void *, int *);

int main(int argc, char ** argv){
	printf("This is init\r\n");

	// Load testbin.bin and run
	void * tarstart = (void *) argv[1];
	int testbin_length;
	void * testbin = tar_getfile("testbin.bin", (unsigned int)tarstart, &testbin_length);
	squire_syscall_process(testbin, testbin_length);
	
}
