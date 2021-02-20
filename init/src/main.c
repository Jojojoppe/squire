#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>
#include <squire_vfs.h>

#include "tar.h"
#include "devman/devman.h"
#include "vfs/vfs.h"

extern int init_fsdrivers_start(void * p);

int main(int argc, char ** argv){
	printf("Main thread of init.bin, argc=%d\r\n", argc);

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	
	// Start Virtual File System
	thrd_t thrd_vfs;
	thrd_create(&thrd_vfs, vfs_main, 0);

	// Start init_fsdrivers
	thrd_t thrd_fsdriver;
	thrd_create(&thrd_fsdriver, init_fsdrivers_start, 0);

	// Wait some time to let VFS and init_fsdrivers start up
	for(unsigned int i=0; i<0x2000000; i++);
	// Mount initramfs
	squire_vfs_mount(0, "initramfs", "", tar_start, VFS_PERMISSIONS_EXECALL|VFS_PERMISSIONS_EXECOWN|VFS_PERMISSIONS_READALL|VFS_PERMISSIONS_READOWN);

	unsigned int fid_LICENCE;
	int ret = squire_vfs_open(0, "/", "LICENCE", VFS_FILE_READ, &fid_LICENCE);
	printf("squire_vfs_open() returned %08x, fid=%08x\r\n", ret, fid_LICENCE);

	for(unsigned int i=0; i<0x2000000; i++);
	ret = squire_vfs_close(&fid_LICENCE);
	printf("squire_vfs_close() returned %08x\r\n", ret);

	for(;;);
	return 0;
} 
