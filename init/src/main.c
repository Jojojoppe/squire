#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>

#include "tar.h"
#include "devman/devman.h"
#include "vfs/vfs.h"

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	
	/*
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
	*/

	// Start Virtual File System
	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	thrd_t thrd_vfs;
	thrd_create(&thrd_vfs, vfs_main, tar_start);

	// Register init as initramfs driver and mount on mp1
	size_t msglen = sizeof(vfs_message_t)+2*sizeof(vfs_submessage_t)+sizeof(vfs_submessage_reg_fsdriver_t)+sizeof(vfs_submessage_mount_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 2;
	msg->messages[0].type = SUBMESSAGE_TYPE_REG_FSDRIVER;
	msg->messages[0].size = sizeof(vfs_submessage_reg_fsdriver_t);
	vfs_submessage_reg_fsdriver_t * content = (vfs_submessage_reg_fsdriver_t*)msg->messages[0].content;
	content->box = 10;
	content->pid = 1;
	strcpy(content->name, "initramfs");
	vfs_submessage_t * mntsubmsg = (vfs_submessage_t*)((void*)msg->messages + msg->messages[0].size + sizeof(vfs_submessage_t));
	mntsubmsg->type = SUBMESSAGE_TYPE_MOUNT;
	mntsubmsg->size = sizeof(vfs_submessage_mount_t);
	vfs_submessage_mount_t * mntmsg = (vfs_submessage_mount_t*)mntsubmsg->content;
	strcpy(mntmsg->fsname, "initramfs");
	mntmsg->mountpoint = 1;
	mntmsg->permissions = PERMISSIONS_READALL|PERMISSIONS_READOWN|PERMISSIONS_EXECALL|PERMISSIONS_EXECOWN;
	strcpy(mntmsg->device_id, "");
	mntmsg->device_instance = 0;
	squire_message_simple_box_send(msg, msglen, 1, 1);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, 10);

	}
	return 0;
} 
