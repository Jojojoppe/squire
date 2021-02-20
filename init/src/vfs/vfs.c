#include <vfs/vfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#include <squire.h>
#include <squire_vfs.h>
#include <squire_fsdriver.h>

vfs_fsdriver_t * fsdriver_list;

vfs_fsdriver_t * fsdriver_find(char * fsname){
	vfs_fsdriver_t * f = fsdriver_list;
	while(f){
		if(!strcmp(f->id, fsname)){
			return f;
		}
		f = f->next;
	}
	return NULL;
}

// -----------------------------------------------------------------------------------

void vfs_register_fsdriver(unsigned int pid, unsigned int box, char * name, uint8_t * id, uint32_t flags, squire_fsdriver_message_t * msg){
	vfs_fsdriver_t * fsdriver = (vfs_fsdriver_t*)malloc(sizeof(vfs_fsdriver_t));
	fsdriver->next = 0;
	strcpy(fsdriver->name, name);
	memcpy(fsdriver->id, id, 64);
	fsdriver->PID = pid;
	fsdriver->simple_box = box;
	fsdriver->flags = flags;
	printf("Added fsdriver definition for '%s' with driver '%s v%d.%d' on %d-%d with flags %08x\r\n", fsdriver->id, fsdriver->name, fsdriver->version_major, fsdriver->version_minor, fsdriver->PID, fsdriver->simple_box, fsdriver->flags);

	if(fsdriver_list){
		vfs_fsdriver_t * l = fsdriver_list;
		while(l->next) l=l->next;
		l->next = fsdriver;
	}else{
		fsdriver_list = fsdriver;
	}
}

// -----------------------------------------------------------------------------------

void vfs_mount(unsigned int pid, unsigned int box, unsigned int mountpoint, unsigned int device_instance, unsigned int permissions, char * fsname, uint8_t * device_id){
	printf("VFS] MOUNT '%s-%08x' at mp%d [%s/%08x]\r\n", device_id, device_instance, mountpoint, fsname, permissions);

	squire_fsdriver_message_t msg;
	msg.function = FSDRIVER_FUNCTION_MOUNT;
	msg.pid = pid;
	msg.box = box;
	msg.uint0 = mountpoint;
	msg.uint1 = device_instance;
	strcpy(msg.id, fsname);
	strcpy(msg.string0, device_id);
	
	// Find driver
	vfs_fsdriver_t * fsdriver = fsdriver_find(fsname);
	if(fsdriver){
		squire_message_simple_box_send(&msg, sizeof(msg), fsdriver->PID, fsdriver->simple_box);
	}else{
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_CANNOT_MOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
}

void vfs_mount_r(unsigned int status, unsigned int box, unsigned pid){
	squire_vfs_message_t msg;
	msg.function = VFS_RPC_RETURN_NOERR;
	squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
}

// -----------------------------------------------------------------------------------

int vfs_fsdriver_main(void * p){
	printf("Starting VFS-FSDRIVER interface\r\n");

	// MESSAGE RECEIVING FOR FSDRIVER INTERFACE
	squire_fsdriver_message_t msg;
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = sizeof(msg);
		squire_message_status_t status = squire_message_simple_box_receive(&msg, &length, &from, RECEIVE_BLOCKED, VFS_FSDRIVER_BOX);

		switch(msg.function){
		
			case FSDRIVER_FUNCTION_REGFSDRIVER_R: {
				unsigned int pid = msg.uint2;
				unsigned int box = msg.uint1;
				unsigned int flags = msg.uint0;
				char * fsname = msg.string1;
				char * fsdrivername = msg.string0;
				vfs_register_fsdriver(pid, box, fsdrivername, fsname, flags, &msg);
			} break;

			case FSDRIVER_FUNCTION_MOUNT_R:{
				unsigned int status = msg.uint0;
				unsigned int pid = msg.pid;
				unsigned int box = msg.box;
				vfs_mount_r(status, box, pid);					   
			} break;

			default:
				printf("VFS-FSDRIVER] Unknown function %d\r\n", msg.function);
				break;
		}

	}
	return 0;
}

int vfs_main(void * p){
	thrd_t thrd_fsdriver;
	thrd_fsdriver = thrd_create(&thrd_fsdriver, vfs_fsdriver_main, 0);

	printf("Starting VFS interface\r\n");

	// MESSAGE RECEIVING FOR VFS INTERFACE
	squire_vfs_message_t msg;
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = sizeof(msg);
		squire_message_status_t status = squire_message_simple_box_receive(&msg, &length, &from, RECEIVE_BLOCKED, VFS_BOX);
		if(status) continue;

		switch(msg.function){
		
			case VFS_RPC_FUNCTION_MOUNT: {
				unsigned int mountpoint = msg.uint0;
				unsigned int device_instance = msg.uint1;
				unsigned int permissions = msg.uint2;
				char * fsname = msg.string0;
				char * device_id = msg.string1;
				vfs_mount(from, msg.box, mountpoint, device_instance, permissions, fsname, device_id);
			} break;

			default:
				printf("VFS] Unknown function %d\r\n", msg.function);
				// Send ERROR
				msg.uint0 = VFS_RPC_RETURN_ERR;
				squire_message_simple_box_send(&msg, sizeof(msg), from, msg.box);
				break;
		}

	}
	return 0;
}

