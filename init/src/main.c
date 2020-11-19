#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>
#include <squire_fsdriver.h>

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
	size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_mount_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 1;
	vfs_submessage_t * mntsubmsg = msg->messages;
	mntsubmsg->type = SUBMESSAGE_TYPE_MOUNT;
	mntsubmsg->size = sizeof(vfs_submessage_mount_t);
	vfs_submessage_mount_t * mntmsg = (vfs_submessage_mount_t*)mntsubmsg->content;
	mntmsg->mountpoint = 1;
	mntmsg->owner = 0;
	strcpy(mntmsg->fsname, "initramfs");
	mntmsg->permissions = PERMISSIONS_READALL|PERMISSIONS_READOWN|PERMISSIONS_EXECALL|PERMISSIONS_EXECOWN;
	squire_message_simple_box_send(msg, msglen, 1, 1);
	free(msg);

	for(;;);
	return 0;
} 
