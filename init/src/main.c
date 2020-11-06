#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>

#include "tar.h"
#include "devman/devman.h"

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	// Get current process information
	unsigned int PID = squire_procthread_getpid();

	// Load x86_generic driver: x86_generic.bin [PID] 0
	// Rest of the drivers are loaded by the device manager, this one is
	// configuration specific
	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	unsigned int length;
	void * x86_generic = tar_get(tar_start, "x86_generic.bin", &length);
	char * x86_generic_argv0 = "x86_generic.bin";
	char * x86_generic_argv1[16];
	sprintf(x86_generic_argv1, "%d", PID);
	char * x86_generic_argv2[16];
	sprintf(x86_generic_argv2, "%d", MSG_BOX_DEVMAN_SIMPLE);
	char ** x86_generic_argv[3];
	x86_generic_argv[0] = x86_generic_argv0;
	x86_generic_argv[1] = x86_generic_argv1;
	x86_generic_argv[2] = x86_generic_argv2;
	unsigned int x86_generic_PID = squire_procthread_create_process(x86_generic, length, 3, x86_generic_argv);

	// Start device manager
	thrd_t thrd_devman;
	thrd_create(&thrd_devman, devman_main, "x86_generic");

	for(;;);
	return 0;
} 
