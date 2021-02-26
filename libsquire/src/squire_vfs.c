#include <squire.h>
#include <squire_rpc.h>
#include <squire_vfs.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <threads.h>

extern squire_vfs_driver_t * __vfs_driver_info;

int squire_vfs_driver_main_direct(int argc, char ** argv, squire_vfs_driver_t * driver_info){
    // Register driver
    size_t message_size = sizeof(squire_vfs_message_header_t) + sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_driver_t);
    size_t submessage_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_driver_t);
    uint8_t * msg = (uint8_t *) malloc(message_size);
    // Fill message content
    squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg;
    hdr->messages = 1;
    hdr->length = message_size;
    squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
    smsg_hdr->submessage_type = SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER;
    smsg_hdr->length = submessage_size;
    squire_vfs_driver_t * smsg_content = (squire_vfs_driver_t*)(smsg_hdr+1);
    driver_info->pid = squire_procthread_getpid();
    memcpy(smsg_content, driver_info, sizeof(squire_vfs_driver_t));
    squire_message_simple_box_send(msg, message_size, SQUIRE_VFS_PID, SQUIRE_VFS_DRIVER_BOX);
    free(msg);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, driver_info->box);

        squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg_buffer;
        squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

				case SQUIRE_VFS_SUBMESSAGE_MOUNT:{
					squire_vfs_submessage_mount_t * mnt = (squire_vfs_submessage_mount_t*)(smsg_hdr+1);
					if(driver_info->mount){
						int r = driver_info->mount(mnt->type, mnt->device, mnt->mountpoint, 0);
						// Send return message
						size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_mount_t);
						size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						memset(rmsg, 0, rmsg_size);
						rmsg->length = rmsg_size;
						rmsg->messages = 1;
						squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						smsg_header->length = smsg_size;
						smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_MOUNT_R;
						squire_vfs_submessage_mount_t * mount = (squire_vfs_submessage_mount_t*)(smsg_header+1);
						memcpy(mount, mnt, sizeof(squire_vfs_submessage_mount_t));
						mount->status = r;
						squire_message_simple_box_send(rmsg, rmsg_size, from, SQUIRE_VFS_DRIVER_BOX);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_OPENDIR:{
					squire_vfs_submessage_dir_t * dir = (squire_vfs_submessage_dir_t*)(smsg_hdr+1);
					if(driver_info->opendir){
						struct dirent dirent;
						int r = driver_info->opendir(dir->path, &dirent);
						// Send return message
						size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
						size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						memset(rmsg, 0, rmsg_size);
						rmsg->length = rmsg_size;
						rmsg->messages = 1;
						squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						smsg_header->length = smsg_size;
						smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR_R;
						squire_vfs_submessage_dir_t * dr = (squire_vfs_submessage_dir_t*)(smsg_header+1);
						memcpy(dr, dir, sizeof(squire_vfs_submessage_dir_t));
						memcpy(&dr->dirent, &dirent, sizeof(struct dirent));
						dr->status = r;
						squire_message_simple_box_send(rmsg, rmsg_size, dr->pid, dr->box);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_READDIR:{
					squire_vfs_submessage_dir_t * dir = (squire_vfs_submessage_dir_t*)(smsg_hdr+1);
					if(driver_info->opendir){
						struct dirent dirent;
						int r = driver_info->readdir(dir->dirent.d_fileno, &dirent);
						// Send return message
						size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
						size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						memset(rmsg, 0, rmsg_size);
						rmsg->length = rmsg_size;
						rmsg->messages = 1;
						squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						smsg_header->length = smsg_size;
						smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_READDIR_R;
						squire_vfs_submessage_dir_t * dr = (squire_vfs_submessage_dir_t*)(smsg_header+1);
						memcpy(dr, dir, sizeof(squire_vfs_submessage_dir_t));
						memcpy(&dr->dirent, &dirent, sizeof(struct dirent));
						dr->status = r;
						squire_message_simple_box_send(rmsg, rmsg_size, dr->pid, dr->box);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_OPEN:{
					squire_vfs_submessage_file_t * file = (squire_vfs_submessage_file_t*)(smsg_hdr+1);
					if(driver_info->opendir){
						unsigned int fd;
						int r = driver_info->open(file->path, &fd);
						// Send return message
						size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_file_t);
						size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						memset(rmsg, 0, rmsg_size);
						rmsg->length = rmsg_size;
						rmsg->messages = 1;
						squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						smsg_header->length = smsg_size;
						smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR_R;
						squire_vfs_submessage_file_t * fl = (squire_vfs_submessage_file_t*)(smsg_header+1);
						memcpy(fl, file, sizeof(squire_vfs_submessage_file_t));
						fl->fdesc = fd;
						fl->status = r;
						squire_message_simple_box_send(rmsg, rmsg_size, fl->pid, fl->box);
						free(rmsg);
					}
				} break;

                default:
                    break;
            }
            smsg_hdr = (squire_vfs_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        };
	}
	return EXIT_SUCCESS;
}

int squire_vfs_driver_main(int argc, char ** argv){
	squire_vfs_driver_main_direct(argc, argv, __vfs_driver_info);
}

int squire_vfs_user_mount(char * type, char * device, unsigned int mountpoint){
	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_mount_t);
	size_t msg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * msg = (squire_vfs_message_header_t*)malloc(msg_size);
	memset(msg, 0, msg_size);
	msg->length = msg_size;
	msg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(msg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_MOUNT;
	squire_vfs_submessage_mount_t * mount = (squire_vfs_submessage_mount_t*)(smsg_header+1);
	mount->mountpoint = mountpoint;
	strcpy(mount->device, device);
	strcpy(mount->type, type);
	mount->box = 255;
	mount->pid = squire_procthread_getpid();

	squire_rpc_box(255, SQUIRE_VFS_PID, SQUIRE_VFS_USER_BOX, msg, msg_size, msg, msg_size);

	int status = mount->status;
	free(msg);
	return status;
}