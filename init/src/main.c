#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>

#include "tar.h"
#include "devman/devman.h"
#include "fsdriver_initramfs.h"
#include "vfs/vfs.h"

int main(int argc, char ** argv){
	printf("Main thread of init.bin, argc=%d\r\n", argc);

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	
	// Start Virtual File System
	thrd_t thrd_vfs;
	thrd_create(&thrd_vfs, vfs_main, 0);

	thrd_t thrd_initramfs;
	thrd_create(&thrd_initramfs, initramfs_fsdriver, tar_start);

	for(int i=0; i<100000000; i++);

	// MOUNT INITRAMFS
	squire_vfs_mount(1, 0, PERMISSIONS_READALL|PERMISSIONS_EXECALL|PERMISSIONS_READOWN|PERMISSIONS_EXECOWN, "initramfs", "", 0);

	// Start Device manager
	thrd_t thrd_devman;
	thrd_create(&thrd_devman, devman_main, "x86_generic");

	// Start root device driver
	unsigned int root_device_driver_size;
	void * root_device_driver = tar_get(tar_start, "x86_generic.bin", &root_device_driver_size);
	char ** rdd_argv[3];
	rdd_argv[0] = "x86_generic";
	rdd_argv[1] = "1";
	rdd_argv[2] = "0";
	squire_procthread_create_process(root_device_driver, root_device_driver_size, 3, rdd_argv);

	for(;;);
	return 0;
} 
