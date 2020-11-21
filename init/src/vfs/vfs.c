#include <vfs/vfs.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_fsdriver.h>

// List of mountpoints
vfs_mountpoint_t * vfs_mountpoints[256] = {0};
// List of filesystem drivers
vfs_fsdriver_t * vfs_fsdrivers = 0;

/**
 * Get a filesystem driver info structure
 */
vfs_fsdriver_t * vfs_find_fsdriver(char name[32]){
	vfs_fsdriver_t * d = vfs_fsdrivers;
	while(d){
		if(!strcmp(d->name, name)){
			return d;
		}
		d = d->next;
	}
	return 0;
}

/**
 * Mount a filesystem
 */
void vfs_mount(vfs_submessage_mount_t * msg_mount){
	if(vfs_mountpoints[msg_mount->mountpoint]){
		printf("VFS] mp%d already mounted\r\n", msg_mount->mountpoint);
		return;
	}

	// Find fsdriver
	vfs_fsdriver_t * fsdriver = vfs_find_fsdriver(msg_mount->fsname);
	if(!fsdriver){
		printf("VFS] filesystem driver '%s' is not registered\r\n", msg_mount->fsname);
		return;
	}

	vfs_mountpoints[msg_mount->mountpoint] = (vfs_mountpoint_t*)malloc(sizeof(vfs_mountpoint_t));
	vfs_mountpoint_t * mp = &vfs_mountpoints[msg_mount->mountpoint];
	mp->fsdriver = fsdriver;
	mp->permissions = msg_mount->permissions;
	mp->device_instance = msg_mount->device_instance;
	memcpy(mp->device_id, msg_mount->device_id, 64);
	mp->rootnode = 0;
	mp->owner = msg_mount->owner;

	// Send FSMOUNT to right fsdriver
	size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_fsmount_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 1;
	vfs_submessage_t * submsg_fsmount = msg->messages;
	submsg_fsmount->type = SUBMESSAGE_TYPE_FSMOUNT;
	submsg_fsmount->size = sizeof(vfs_submessage_fsmount_t);
	vfs_submessage_fsmount_t * fsmount = (vfs_submessage_fsmount_t*)submsg_fsmount->content;
	fsmount->mountpoint = msg_mount->mountpoint;
	fsmount->device_instance = mp->device_instance;
	strcpy(fsmount->fsname, mp->fsdriver->name);
	memcpy(fsmount->device_id, mp->device_id, 64);
	squire_message_simple_box_send(msg, msglen, mp->fsdriver->pid, mp->fsdriver->box);
	free(msg);

	printf("VFS] '%s' filesystem is being mounted on mp%d with permissions %04x\r\n", mp->fsdriver->name, msg_mount->mountpoint, mp->permissions);
}
void vfs_fsmount_r(vfs_submessage_fsmount_r_t * msg_mount){
	vfs_mountpoint_t * mp = &vfs_mountpoints[msg_mount->mountpoint];
	if(!mp){
		printf("VFS] mp%d is not in mounting process\r\n", msg_mount->mountpoint);
		return;
	}
	if(mp->rootnode){
		printf("VFS] mp%d already mounted\r\n", msg_mount->mountpoint);
		return;
	}
	mp->fsdriver_private = msg_mount->fsdriver_private_mount;

	// Create root node
	mp->rootnode = (vfs_node_t*)malloc(sizeof(vfs_node_t));
	mp->rootnode->childs = 0;
	mp->rootnode->parent = 0;
	mp->rootnode->next = 0;
	mp->rootnode->prev = 0;
	mp->rootnode->fsdriver_private = msg_mount->fsdriver_private_root;
	mp->rootnode->permissions = mp->permissions;
	mp->rootnode->owner = mp->owner;

	printf("VFS] '%s' filesystem mounted on mp%d with permissions %04x\r\n", mp->fsdriver->name, msg_mount->mountpoint, mp->permissions);

	// TODO DEBUG
	// Send FSlsdir
	size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_fslsdir_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 1;
	vfs_submessage_t * submsg_fsmount = msg->messages;
	submsg_fsmount->type = SUBMESSAGE_TYPE_FSLSDIR;
	submsg_fsmount->size = sizeof(vfs_submessage_fslsdir_t);
	vfs_submessage_fslsdir_t * fslsdir = (vfs_submessage_fslsdir_t*)submsg_fsmount->content;
	fslsdir->fsdriver_private_mount = mp->fsdriver_private;
	fslsdir->fsdriver_private = mp->rootnode->fsdriver_private;
	squire_message_simple_box_send(msg, msglen, mp->fsdriver->pid, mp->fsdriver->box);
	free(msg);

}

/**
 * Register a filesystem driver
 */
void vfs_register_fsdriver(vfs_submessage_reg_fsdriver_t * msg_fsdriver){
	vfs_fsdriver_t * fsdriver = (vfs_fsdriver_t*)malloc(sizeof(vfs_fsdriver_t));
	fsdriver->next = 0;
	fsdriver->pid = msg_fsdriver->pid;
	fsdriver->box = msg_fsdriver->box;
	fsdriver->flags = msg_fsdriver->flags;
	strcpy(fsdriver->name, msg_fsdriver->name);
	if(vfs_fsdrivers){
		vfs_fsdriver_t * f = vfs_fsdrivers;
		while(f->next) f = f->next;
		f->next = fsdriver;
	}else{
		vfs_fsdrivers = fsdriver;
	}
	printf("VFS] registerd filesystem driver '%s' at %d:%d with flags %02x [", fsdriver->name, fsdriver->pid, fsdriver->box, fsdriver->flags);
	if(fsdriver->flags&VFS_FSDRIVER_FLAGS_NODEVICE)
		printf("NODEVICE,");
	if(fsdriver->flags&VFS_FSDRIVER_FLAGS_NOCACHE)
		printf("NOCACHE,");
	printf("]\r\n");
}

/**
 * List a filesystem directory
 */
void vfs_fslsdir_r(vfs_submessage_fslsdir_r_t * msg_fslsdir, unsigned int from){
	vfs_lsdir_node_t * shared_nodelist = (vfs_lsdir_node_t*) squire_memory_map_shared(0, from, msg_fslsdir->shared_id, MMAP_READ);
	for(int i=0; i<msg_fslsdir->nr_nodes; i++){
		printf("lsdir> %s\r\n", shared_nodelist[i].name);
	}
	squire_memory_munmap(shared_nodelist, msg_fslsdir->shared_length);
}

int vfs_main(void * p){
	printf("Starting VFS\r\n");

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, MSG_BOX_VFS_SIMPLE);

		vfs_message_t * msg = (vfs_message_t*) msg_buffer;
		vfs_submessage_t * submsg = msg->messages;
		for(int i=0; i<msg->amount_messages; i++){
			switch(submsg->type){
			
				case SUBMESSAGE_TYPE_REG_FSDRIVER:{
					vfs_submessage_reg_fsdriver_t * content = (vfs_submessage_reg_fsdriver_t*)submsg->content;
					vfs_register_fsdriver(content);
				} break;

				case SUBMESSAGE_TYPE_MOUNT:{
					vfs_submessage_mount_t * content = (vfs_submessage_mount_t*)submsg->content;
					vfs_mount(content);
				} break;

				case SUBMESSAGE_TYPE_FSMOUNT_R:{
					vfs_submessage_fsmount_r_t * content = (vfs_submessage_fsmount_r_t*)submsg->content;
					vfs_fsmount_r(content);
				} break;

				case SUBMESSAGE_TYPE_FSLSDIR_R:{
					vfs_submessage_fslsdir_r_t * content = (vfs_submessage_fslsdir_r_t*)submsg->content;
					vfs_fslsdir_r(content, from);
				} break;

				default:
					break;
			}
			submsg = (vfs_submessage_t*)((void*)submsg + submsg->size + sizeof(vfs_submessage_t));
		}

	}
	return 0;
}

