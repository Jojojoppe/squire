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

void * tar_start;

void lsdir(vfs_submessage_fslsdir_t* msg_lsdir){
	// TODO use private_mount and private


	// List over tar to get nodelist size
	tar_header_t * hdr = (tar_header_t*) tar_start;
	unsigned int nodelist_size = 0;
	while(hdr->filename[0]){

		nodelist_size += sizeof(vfs_lsdir_node_t);
	
		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}

	// Get shared size
	size_t shared_size = ((nodelist_size/4096 + 1)*4096);
	vfs_lsdir_node_t * shared_nodelist = (vfs_lsdir_node_t*) squire_memory_create_shared(0, shared_size, "FSLSDIR_0123456789", MMAP_READ|MMAP_WRITE);
	
	// Fill nodelist
	hdr = (tar_header_t*) tar_start;
	unsigned int i = 0;
	while(hdr->filename[0]){

		shared_nodelist[i].fsdriver_private = (void*)hdr;
		shared_nodelist[i].owner = 0;
		shared_nodelist[i].permissions = PERMISSIONS_EXECALL|PERMISSIONS_EXECOWN|PERMISSIONS_READALL|PERMISSIONS_READOWN;
		shared_nodelist[i].type = VFS_NODE_TYPE_FILE;
		strcpy(shared_nodelist[i].name, hdr->filename);

		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
		i++;
	}

	// Send FSLSDIR_R back to VFS
	size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_fslsdir_r_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 1;
	vfs_submessage_t * submsg_fsmount = msg->messages;
	submsg_fsmount->type = SUBMESSAGE_TYPE_FSLSDIR_R;
	submsg_fsmount->size = sizeof(vfs_submessage_fslsdir_t);
	vfs_submessage_fslsdir_r_t * fslsdir_r = (vfs_submessage_fslsdir_r_t*)submsg_fsmount->content;
	fslsdir_r->nr_nodes = i;
	fslsdir_r->shared_length = shared_size;
	strcpy(fslsdir_r->shared_id, "FSLSDIR_0123456789");
	squire_message_simple_box_send(msg, msglen, 1, 1);
	free(msg);

	squire_memory_transfer_shared("FSLSDIR_0123456789");
	squire_memory_munmap(shared_nodelist, shared_size);

}

int main(int argc, char ** argv){
	printf("Main thread of init.bin, argc=%d\r\n", argc);

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	tar_start = (void*)(*((unsigned int*)argv[1]));
	
	// Start Virtual File System
	thrd_t thrd_vfs;
	thrd_create(&thrd_vfs, vfs_main, 0);

	// Register init as initramfs driver and mount on mp1
	size_t msglen = sizeof(vfs_message_t)+2*sizeof(vfs_submessage_t)+sizeof(vfs_submessage_reg_fsdriver_t)+sizeof(vfs_submessage_mount_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 2;

	msg->messages[0].type = SUBMESSAGE_TYPE_REG_FSDRIVER;
	msg->messages[0].size = sizeof(vfs_submessage_reg_fsdriver_t);
	vfs_submessage_reg_fsdriver_t * content = (vfs_submessage_reg_fsdriver_t*)msg->messages[0].content;
	content->box = 10;
	content->pid = 1;
	content->flags = VFS_FSDRIVER_FLAGS_NODEVICE | VFS_FSDRIVER_FLAGS_NOCACHE;
	strcpy(content->name, "initramfs");

	vfs_submessage_t * mntsubmsg = (vfs_submessage_t*)((void*)msg->messages + msg->messages[0].size + sizeof(vfs_submessage_t));
	mntsubmsg->type = SUBMESSAGE_TYPE_MOUNT;
	mntsubmsg->size = sizeof(vfs_submessage_mount_t);
	vfs_submessage_mount_t * mntmsg = (vfs_submessage_mount_t*)mntsubmsg->content;
	mntmsg->mountpoint = 1;
	mntmsg->owner = 0;
	strcpy(mntmsg->fsname, "initramfs");
	mntmsg->permissions = PERMISSIONS_READALL|PERMISSIONS_READOWN|PERMISSIONS_EXECALL|PERMISSIONS_EXECOWN;

	squire_message_simple_box_send(msg, msglen, 1, 1);
	free(msg);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, 10);

		vfs_message_t * msg = (vfs_message_t*) msg_buffer;
		vfs_submessage_t * submsg = msg->messages;
		for(int i=0; i<msg->amount_messages; i++){
			switch(submsg->type){
			
				case SUBMESSAGE_TYPE_FSMOUNT:{
					vfs_submessage_fsmount_t * fsmount = (vfs_submessage_fsmount_t*)submsg->content;

					// Send FSMOUNT_R back to VFS
					size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_fsmount_r_t);
					vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
					msg->amount_messages = 1;
					vfs_submessage_t * submsg_fsmount = msg->messages;
					submsg_fsmount->type = SUBMESSAGE_TYPE_FSMOUNT_R;
					submsg_fsmount->size = sizeof(vfs_submessage_fsmount_t);
					vfs_submessage_fsmount_r_t * fsmount_r = (vfs_submessage_fsmount_r_t*)submsg_fsmount->content;

					fsmount_r->mountpoint = fsmount->mountpoint;
					fsmount_r->fsdriver_private_mount = 0;
					fsmount_r->fsdriver_private_root = 0;

					squire_message_simple_box_send(msg, msglen, 1, 1);
					free(msg);

				} break;

				case SUBMESSAGE_TYPE_FSLSDIR:{
					vfs_submessage_fslsdir_t * fslsdir = (vfs_submessage_fslsdir_t*) submsg->content;
					lsdir(fslsdir);
				} break;

				default:
					break;
			}
			submsg = (vfs_submessage_t*)((void*)submsg + submsg->size + sizeof(vfs_submessage_t));
		}


	}
	return 0;
} 
