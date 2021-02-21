#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>
#include <squire_vfs.h>

#include "tar.h"
#include "DDM/ddm.h"

extern int init_fsdrivers_start(void * p);

int main(int argc, char ** argv){
	printf("Main thread of init.bin, argc=%d\r\n", argc);

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	void * tar_start = (void*)(*((unsigned int*)argv[1]));

	// Start DDM
	thrd_t thrd_ddm;
	thrd_create(&thrd_ddm, ddm_main, 0);
	
	unsigned int root_device_driver_size;
	void * root_device_driver = tar_get(tar_start, "x86_generic.bin", &root_device_driver_size);
	char ** rdd_argv[1];
	rdd_argv[0] = "x86_generic";
	squire_procthread_create_process(root_device_driver, root_device_driver_size, 1, rdd_argv);

	for(;;);
	return 0;
} 
