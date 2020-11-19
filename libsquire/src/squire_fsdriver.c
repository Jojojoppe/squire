#include <squire.h>
#include <squire_fsdriver.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

unsigned int vfs_pid = 0;
unsigned int vfs_box = 0;

extern SQUIRE_FSDRIVER_INFO * _fsdriver_info;

int fsdriver_main(int argc, char ** argv){

	// Get the paramters
	if(argc!=3){
		printf("FSdriver must be executed with 3 parameters: name VFS_PID VFS_box\r\n");
		return EXIT_FAILURE;
	}
	char * param_name = argv[0];
	vfs_pid = atoi(argv[1]);
	vfs_box = atoi(argv[2]);

	// Calculate fsdriver registration message size
	size_t msglen = sizeof(vfs_message_t);
	unsigned int supported = 0;
	squire_fsdriver_supported_t * sup = _fsdriver_info->fsdriver_info->supported;
	while(strlen(sup->fsname)!=0){
		supported++;
		msglen += sizeof(vfs_submessage_t)+sizeof(vfs_submessage_reg_fsdriver_t);
		sup += 1;
	}

	// Register FSdriver
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = supported;

	vfs_submessage_t * submsg = msg->messages;
	for(int i=0; i<supported; i++){
		submsg->type = SUBMESSAGE_TYPE_REG_FSDRIVER;
		submsg->size = sizeof(vfs_submessage_reg_fsdriver_t);
		vfs_submessage_reg_fsdriver_t * content = (vfs_submessage_reg_fsdriver_t*)submsg->content;
		content->box = _fsdriver_info->fsdriver_info->simple_box;
		content->pid = squire_procthread_getpid();
		content->flags = _fsdriver_info->fsdriver_info->supported[i].flags;
		strcpy(content->name, _fsdriver_info->fsdriver_info->supported[i].fsname);
		submsg = (vfs_submessage_t*)((void*)submsg + submsg->size + sizeof(vfs_submessage_t));
	}

	squire_message_simple_box_send(msg, msglen, vfs_pid, vfs_box);
	free(msg);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, _fsdriver_info->fsdriver_info->simple_box);

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

					_fsdriver_info->fsmount_callback(&fsmount_r->fsdriver_private_mount, &fsmount_r->fsdriver_private_root, fsmount->mountpoint, fsmount->fsname, fsmount->device_id, fsmount->device_instance);
					fsmount_r->mountpoint = fsmount->mountpoint;

					squire_message_simple_box_send(msg, msglen, vfs_pid, vfs_box);
					free(msg);

				} break;

				case SUBMESSAGE_TYPE_FSLSDIR:{
					vfs_submessage_fslsdir_t * fslsdir = (vfs_submessage_fslsdir_t*)submsg->content;
					// Generate random shared ID
					char shared_id[32] = "RANDOM_SHARED_ID";
					void * shared_nodelist = 0;

					// Send FSLSDIR_R back to VFS
					size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_fslsdir_r_t);
					vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
					msg->amount_messages = 1;
					vfs_submessage_t * submsg_fsmount = msg->messages;
					submsg_fsmount->type = SUBMESSAGE_TYPE_FSLSDIR_R;
					submsg_fsmount->size = sizeof(vfs_submessage_fslsdir_t);
					vfs_submessage_fslsdir_r_t * fslsdir_r = (vfs_submessage_fslsdir_r_t*)submsg_fsmount->content;

					_fsdriver_info->fslsdir_callback(&fslsdir_r->nr_nodes, &fslsdir_r->shared_length, &shared_nodelist, fslsdir->fsdriver_private_mount, fslsdir->fsdriver_private, shared_id);

					strcpy(fslsdir_r->shared_id, shared_id);
					squire_message_simple_box_send(msg, msglen, vfs_pid, vfs_box);
					free(msg);

					squire_memory_transfer_shared(shared_id);
					squire_memory_munmap(shared_nodelist, fslsdir_r->shared_length);

				} break;
			
				default:
					break;
			}
			submsg = (vfs_submessage_t*)((void*)submsg + submsg->size + sizeof(vfs_submessage_t));
		}


	}

	return EXIT_SUCCESS;
}
