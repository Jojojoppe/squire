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
vfs_mountpoint_t vfs_mountpoints[256];

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
	fsdriver->version_major = msg->uint3>>16;
	fsdriver->version_minor = msg->uint3;
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
	// Check if valid mountpoint
	if(mountpoint>=256){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_CANNOT_MOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
	// Check if already in use
	if(vfs_mountpoints[mountpoint].fsdriver!=NULL){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_ALREADY_MOUNTED;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}

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
		msg.function = VFS_RPC_RETURN_NOERR;
		msg.uint0 = VFS_RPC_RETURN_CANNOT_MOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
}

void vfs_mount_r(unsigned int status, unsigned int mountpoint, void * private_mount, void * private_root, uint8_t * fsname, unsigned int box, unsigned pid){
	if(status!=VFS_RPC_RETURN_NOERR){
		// ERROR
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = status;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}

	// Fill mountpoint data
	vfs_mountpoints[mountpoint].fsdriver = fsdriver_find(fsname);
	vfs_mountpoints[mountpoint].private_mount = private_mount;
	vfs_mountpoints[mountpoint].private_root = private_root;

	// Return NOERR
	squire_vfs_message_t msg;
	msg.function = VFS_RPC_RETURN_NOERR;
	msg.uint0 = status;
	squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
}

// -----------------------------------------------------------------------------------

void vfs_unmount(unsigned int pid, unsigned int box, unsigned int mountpoint){

	// Check if valid mountpoint
	if(mountpoint>=256){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_CANNOT_UNMOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
	// Check if in use
	if(vfs_mountpoints[mountpoint].fsdriver==NULL){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_NOT_MOUNTED;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}

	squire_fsdriver_message_t msg;
	msg.function = FSDRIVER_FUNCTION_UNMOUNT;
	msg.pid = pid;
	msg.box = box;
	msg.uint0 = mountpoint;
	msg.voidp0 = vfs_mountpoints[mountpoint].private_mount;
	strcpy(msg.id, vfs_mountpoints[mountpoint].fsdriver->id);

	squire_message_simple_box_send(&msg, sizeof(msg), vfs_mountpoints[mountpoint].fsdriver->PID, vfs_mountpoints[mountpoint].fsdriver->simple_box);
}

void vfs_unmount_r(unsigned int status, unsigned int mountpoint, unsigned int pid, unsigned int box){
	vfs_mountpoints[mountpoint].fsdriver = NULL;

	// Return NOERR
	squire_vfs_message_t msg;
	msg.function = VFS_RPC_RETURN_NOERR;
	msg.uint0 = status;
	squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
}

// -----------------------------------------------------------------------------------

void vfs_open(unsigned int pid, unsigned int box, unsigned int mountpoint, char * path, char * fname, unsigned int operations){
	// Check if valid mountpoint
	if(mountpoint>=256){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_CANNOT_UNMOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
	// Check if in use
	if(vfs_mountpoints[mountpoint].fsdriver==NULL){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_NOT_MOUNTED;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}

	squire_fsdriver_message_t msg;
	msg.function = FSDRIVER_FUNCTION_OPEN;
	msg.pid = pid;
	msg.box = box;
	msg.uint0 = mountpoint;
	msg.voidp0 = vfs_mountpoints[mountpoint].private_mount;
	msg.voidp1 = vfs_mountpoints[mountpoint].private_root;
	strcpy(msg.id, vfs_mountpoints[mountpoint].fsdriver->id);
	strcpy(msg.string0, path);
	strcpy(msg.string1, fname);
	msg.uint1 = operations;

	squire_message_simple_box_send(&msg, sizeof(msg), vfs_mountpoints[mountpoint].fsdriver->PID, vfs_mountpoints[mountpoint].fsdriver->simple_box);
}

void vfs_open_r(unsigned int status, unsigned int mountpoint, unsigned int FID, unsigned int pid, unsigned int box){
	// Return NOERR
	squire_vfs_message_t msg;
	msg.function = VFS_RPC_RETURN_NOERR;
	msg.uint0 = status;
	msg.uint1 = FID;
	squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
}

// -----------------------------------------------------------------------------------

void vfs_close(unsigned int pid, unsigned int box, unsigned int mountpoint, unsigned int fid){
	// Check if valid mountpoint
	if(mountpoint>=256){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_CANNOT_UNMOUNT;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}
	// Check if in use
	if(vfs_mountpoints[mountpoint].fsdriver==NULL){
		squire_vfs_message_t msg;
		msg.function = VFS_RPC_RETURN_ERR;
		msg.uint0 = VFS_RPC_RETURN_NOT_MOUNTED;
		squire_message_simple_box_send(&msg, sizeof(msg), pid, box);
	}

	squire_fsdriver_message_t msg;
	msg.function = FSDRIVER_FUNCTION_CLOSE;
	msg.pid = pid;
	msg.box = box;
	msg.uint0 = mountpoint;
	msg.voidp0 = vfs_mountpoints[mountpoint].private_mount;
	msg.voidp1 = vfs_mountpoints[mountpoint].private_root;
	strcpy(msg.id, vfs_mountpoints[mountpoint].fsdriver->id);
	msg.uint1 = fid;

	squire_message_simple_box_send(&msg, sizeof(msg), vfs_mountpoints[mountpoint].fsdriver->PID, vfs_mountpoints[mountpoint].fsdriver->simple_box);
}

void vfs_close_r(unsigned int status, unsigned int pid, unsigned int box){
	// Return NOERR
	squire_vfs_message_t msg;
	msg.function = VFS_RPC_RETURN_NOERR;
	msg.uint0 = status;
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
				unsigned int mountpoint = msg.uint1;
				void * private_mount = msg.voidp0;
				void * private_root = msg.voidp1;
				uint8_t * fsname = msg.id;
				vfs_mount_r(status, mountpoint, private_mount, private_root, fsname, box, pid);					   
			} break;

			case FSDRIVER_FUNCTION_UNMOUNT_R:{
				unsigned int status = msg.uint0;
				unsigned int mountpoint = msg.uint1;
				unsigned int pid = msg.pid;
				unsigned int box = msg.box;
				vfs_unmount_r(status, mountpoint, pid, box);
			} break;

			case FSDRIVER_FUNCTION_OPEN_R:{
				unsigned int status = msg.uint0;
				unsigned int mountpoint = msg.uint1;
				unsigned int FID = msg.uint2;
				vfs_open_r(status, mountpoint, FID, msg.pid, msg.box);
			} break;

			case FSDRIVER_FUNCTION_CLOSE_R:{
				unsigned int status = msg.uint0;
				vfs_close_r(status, msg.pid, msg.box);
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

			case VFS_RPC_FUNCTION_UNMOUNT:{
				unsigned int mountpoint = msg.uint0;
				vfs_unmount(from, msg.box, mountpoint);
			} break;

			case VFS_RPC_FUNCTION_OPEN:{
				unsigned int mountpoint = msg.uint0;
				unsigned int operations = msg.uint1;
				char * path = msg.string0;
				char * fname = msg.string1;
				vfs_open(from, msg.box, mountpoint, path, fname, operations);
			} break;

			case VFS_RPC_FUNCTION_CLOSE:{
				unsigned int mountpoint = msg.uint0;
				unsigned int fid = msg.uint1;
				vfs_close(from, msg.box, mountpoint, fid);
			} break;

			default:
				printf("VFS] Unknown function %d\r\n", msg.function);
				// Send ERROR
				msg.function = VFS_RPC_RETURN_ERR;
				msg.uint0 = VFS_RPC_RETURN_ERR;
				squire_message_simple_box_send(&msg, sizeof(msg), from, msg.box);
				break;
		}

	}
	return 0;
}

