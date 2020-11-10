#include <vfs/vfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>

// List of mountpoints
vfs_mountpoint_t * vfs_mountpoints[256] = {0};
// List of filesystem drivers
vfs_fsdriver_t * vfs_fsdrivers = 0;

void vfs_mount(vfs_submessage_mount_t * msg_mount){
	if(vfs_mountpoints[msg_mount->mountpoint]){
		printf("VFS] mp%d already mounted\r\n", msg_mount->mountpoint);
		return;
	}

	vfs_mountpoints[msg_mount->mountpoint] = (vfs_mountpoint_t*)malloc(sizeof(vfs_mountpoint_t));
	vfs_mountpoint_t * mp = vfs_mountpoints[msg_mount->mountpoint];
	// Fill in FSdriver info
}

void vfs_register_fsdriver(vfs_submessage_reg_fsdriver_t * msg_fsdriver){
	vfs_fsdriver_t * fsdriver = (vfs_fsdriver_t*)malloc(sizeof(vfs_fsdriver_t));
	fsdriver->next = 0;
	fsdriver->pid = msg_fsdriver->pid;
	fsdriver->box = msg_fsdriver->box;
	strcpy(fsdriver->name, msg_fsdriver->name);
	if(vfs_fsdrivers){
		vfs_fsdriver_t * f = vfs_fsdrivers;
		while(f->next) f = f->next;
		f->next = fsdriver;
	}else{
		vfs_fsdrivers = fsdriver;
	}
	printf("VFS] registerd filesystem driver '%s' at %d:%d\r\n", fsdriver->name, fsdriver->pid, fsdriver->box);
}

int vfs_main(void * p){
	printf("Starting VFS\r\n");

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, MSG_BOX_VFS_SIMPLE);
		printf("status: %d\r\n", status);

		vfs_message_t * msg = (vfs_message_t*) msg_buffer;
		vfs_submessage_t * submsg = msg->messages;
		printf("Submessages: %d\r\n", msg->amount_messages);
		for(int i=0; i<msg->amount_messages; i++){
			switch(submsg->type){
			
				case SUBMESSAGE_TYPE_REG_FSDRIVER:{
					vfs_submessage_reg_fsdriver_t * content = (vfs_submessage_reg_fsdriver_t*)submsg->content;
					vfs_register_fsdriver(content);
				} break;

				case SUBMESSAGE_TYPE_MOUNT:{
					vfs_submessage_mount_t * content = (vfs_submessage_mount_t*)submsg->content;
					printf("Mount %s -> '%s-%08x' at %d with permissions %08x\r\n", content->fsname, content->device_id, content->device_instance, content->mountpoint, content->permissions);
					vfs_mount(content);
				} break;

				default:
					break;
			}
			submsg = (vfs_submessage_t*)((void*)submsg + submsg->size + sizeof(vfs_submessage_t));
		}

	}
	return 0;
}
