#include <squire.h>
#include <squire_rpc.h>
#include <squire_vfs.h>
#include <squire_crypt.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <threads.h>
#include <string.h>

squire_vfs_driver_t * __vfs_driver_info;

typedef struct vfs_opened_file_s{
	unsigned int fd;
	unsigned int pid;
	uint64_t nonce;
	char signature[64];
	struct vfs_opened_file_s * next;
} vfs_opened_file_t;

int memcmp2( const void * s1, const void * s2, size_t n ){
    const unsigned char * p1 = (const unsigned char *) s1;
    const unsigned char * p2 = (const unsigned char *) s2;
    while ( n-- )
    {
        if ( *p1 != *p2 )
        {
            return *p1 - *p2;
        }
        ++p1;
        ++p2;
    }
    return 0;
}

void strcpy3(char * dst, char * src){
    while(*src){
        *dst = *src;
        dst++; src++;
    }
}

void * squire_vfs_create_message(void ** msg, size_t length, char * signature, int type){
    size_t message_size = sizeof(squire_vfs_message_header_t) + sizeof(squire_vfs_submessage_header_t) + length;
    size_t submessage_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_driver_t) + length;
    *msg = (uint8_t *) malloc(message_size);
    squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) *msg;
    hdr->messages = 1;
    hdr->length = message_size;
	strcpy3(hdr->signature, signature); hdr->signature[32] = 0;
    squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
    smsg_hdr->submessage_type = type;
    smsg_hdr->length = submessage_size;
    return (void*)(smsg_hdr+1);
}

int squire_vfs_driver_main_direct(int argc, char ** argv, squire_vfs_driver_t * driver_info){
    // Create message key
	int argseed = argc;
	for(int i=0; i<argc; i++) argc+= argv[i];
    unsigned long long ticks = squire_misc_get_ticks() + rand() + argseed;
    srand(ticks);
    char * characters = "qwertyuiopasdfghjklzxcvbnm1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
    for(int i=0; i<32; i++){
        driver_info->rand_key[i] = characters[rand()%63];
    }
    driver_info->rand_key[32] = 0;

    // Register driver and wait for response
	squire_vfs_message_header_t * msg;
    squire_vfs_driver_t * smsg_content = (squire_vfs_driver_t*) squire_vfs_create_message(&msg, sizeof(squire_vfs_driver_t), driver_info->rand_key, SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER);
    driver_info->pid = squire_procthread_getpid();
    memcpy(smsg_content, driver_info, sizeof(squire_vfs_driver_t));
	squire_rpc_box(driver_info->box, SQUIRE_VFS_PID, SQUIRE_VFS_DRIVER_BOX, msg, VFS_MSG_LEN(sizeof(squire_vfs_driver_t)), msg, VFS_MSG_LEN(sizeof(squire_vfs_driver_t)));
	// Response contains a message with the signature of the VFS encrypted with own signature
	squire_chacha(driver_info->rand_key, 0, 0, msg->signature, 32);
	memcpy(driver_info->vfs_signature, msg->signature, 32);
	driver_info->vfs_signature[32] = 0;
    free(msg);

	// Initialize other internall stuff
	driver_info->openfiles = 0;

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
					if(driver_info->mount){
						squire_vfs_submessage_mount_t * mnt = (squire_vfs_submessage_mount_t*)(smsg_hdr+1);
						// Get signature [sig_v]sig_d and [sig_u]sig_d
						squire_chacha(driver_info->rand_key, 0, 0, hdr->signature, 32);
						squire_chacha(driver_info->rand_key, 0, 0, mnt->signature, 32);

						// Check if signature is correct (should be sig_v)
						int r = -1;
						if(!memcmp2(driver_info->vfs_signature, hdr->signature, 32)){
							// Correct: MOUNT FILESYSTEM
							r = driver_info->mount(mnt->type, mnt->device, mnt->mountpoint, 0);
						}
						// Send return message
						size_t rmsg_len = VFS_MSG_LEN(sizeof(squire_vfs_submessage_mount_t));
						squire_vfs_message_header_t * rmsg;
						squire_vfs_submessage_mount_t * mount = (squire_vfs_submessage_mount_t*) squire_vfs_create_message(&rmsg, rmsg_len, hdr->signature, SQUIRE_VFS_SUBMESSAGE_MOUNT_R);
						memcpy(mount, mnt, sizeof(squire_vfs_submessage_mount_t));
						mount->status = r;
						// Reencrypt signatures
						squire_chacha(driver_info->rand_key, 0, 0, rmsg->signature, 32);
						squire_chacha(driver_info->rand_key, 0, 0, mount->signature, 32);
						squire_message_simple_box_send(rmsg, rmsg_len, SQUIRE_VFS_PID, SQUIRE_VFS_DRIVER_BOX);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_OPENDIR:{
					squire_vfs_submessage_dir_t * dir = (squire_vfs_submessage_dir_t*)(smsg_hdr+1);
					if(driver_info->opendir){
						// struct dirent dirent;
						// int r = -2;
						// // Check if signature is correct (sent by VFS: signature should be own)
						// if(!strcmp(driver_info->rand_key, hdr->signature)){
						// 	r = driver_info->opendir(dir->path, &dirent);
						// }
						// // Send return message
						// size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
						// size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						// squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						// memset(rmsg, 0, rmsg_size);
						// rmsg->length = rmsg_size;
						// rmsg->messages = 1;
						// squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						// smsg_header->length = smsg_size;
						// smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR_R;
						// squire_vfs_submessage_dir_t * dr = (squire_vfs_submessage_dir_t*)(smsg_header+1);
						// memcpy(dr, dir, sizeof(squire_vfs_submessage_dir_t));
						// memcpy(&dr->dirent, &dirent, sizeof(struct dirent));
						// dr->status = r;
						// squire_message_simple_box_send(rmsg, rmsg_size, dr->pid, dr->box);
						// free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_READDIR:{
					squire_vfs_submessage_dir_t * dir = (squire_vfs_submessage_dir_t*)(smsg_hdr+1);
					if(driver_info->opendir){
						// struct dirent dirent;
						// int r = driver_info->readdir(dir->dirent.d_fileno, &dirent);
						// // Send return message
						// size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
						// size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
						// squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
						// memset(rmsg, 0, rmsg_size);
						// rmsg->length = rmsg_size;
						// rmsg->messages = 1;
						// squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
						// smsg_header->length = smsg_size;
						// smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_READDIR_R;
						// squire_vfs_submessage_dir_t * dr = (squire_vfs_submessage_dir_t*)(smsg_header+1);
						// memcpy(dr, dir, sizeof(squire_vfs_submessage_dir_t));
						// memcpy(&dr->dirent, &dirent, sizeof(struct dirent));
						// dr->status = r;
						// squire_message_simple_box_send(rmsg, rmsg_size, dr->pid, dr->box);
						// free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_OPEN:{
					if(driver_info->open){
						squire_vfs_submessage_file_t * file = (squire_vfs_submessage_file_t*)(smsg_hdr+1);
						// Get signature [sig_v]sig_d and [sig_u]sig_d
						squire_chacha(driver_info->rand_key, 0, 0, hdr->signature, 32);
						squire_chacha(driver_info->rand_key, 0, 0, file->signature, 32);

						// Check if signature is correct (should be sig_v)
						int r = -1;
						unsigned int fd;
						if(!memcmp2(driver_info->vfs_signature, hdr->signature, 32)){
							r = driver_info->open(file->path, &fd);

							// Add user PID, signature and nonce to accepted list
							vfs_opened_file_t * o = (vfs_opened_file_t*)malloc(sizeof(vfs_opened_file_t));
							o->next = 0;
							o->nonce = file->nonce;
							o->pid = file->pid;
							o->fd = fd;
							memcpy(o->signature, file->signature, 32);
							if(driver_info->openfiles){
								vfs_opened_file_t * l = driver_info->openfiles;
								while(l->next) l=l->next;
								l->next = o;
							}else{
								driver_info->openfiles = o;
							}
						}
						// Send return message
						size_t rmsg_len = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t));
						squire_vfs_message_header_t * rmsg;
						squire_vfs_submessage_file_t * fl = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&rmsg, rmsg_len, file->signature, SQUIRE_VFS_SUBMESSAGE_OPEN_R);
						memcpy(fl, file, sizeof(squire_vfs_submessage_file_t));
						fl->status = r;
						fl->fdesc = fd;
						// Reencrypt signatures
						memcpy(fl->signature, driver_info->rand_key, 32);
						squire_chacha(file->signature, 0, 0, fl->signature, 32);
						squire_message_simple_box_send(rmsg, rmsg_len, file->pid, file->box);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_READ:{
					if(driver_info->read){
						squire_vfs_submessage_file_t * file = (squire_vfs_submessage_file_t*)(smsg_hdr+1);

						unsigned int fd = file->fdesc;
						size_t length = file->length;
						int r = -1;

						size_t rmsg_size = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t)) + length;
						squire_vfs_message_header_t * rmsg;
						squire_vfs_submessage_file_t * fl = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&rmsg, rmsg_size, driver_info->rand_key, SQUIRE_VFS_SUBMESSAGE_READ_R);
						char * buf = (char*)(fl+1);
						memset(rmsg, 0, rmsg_size);

						// Check if combo fd and pid in list
						vfs_opened_file_t * o = driver_info->openfiles;
						while(o){
							if(o->fd==fd && o->pid==from){
								// Check if signature is correct (should be sig_d)
								// Get signature [sig_d]sig_u|nonce
								squire_chacha(o->signature, 0, o->nonce, hdr->signature, 32);
								if(!memcmp2(driver_info->rand_key, hdr->signature, 32)){
									r = driver_info->read(fd, file->offset, &length, buf);
								}
								break;
							}
							o->next;
						}

						if(r) length=0;
						else o->nonce++;

						memcpy(fl, file, sizeof(squire_vfs_submessage_file_t));
						fl->fdesc = fd;
						fl->status = r;
						fl->length = length;
						memcpy(rmsg->signature, driver_info->rand_key, 32);
						squire_message_simple_box_send(rmsg, rmsg_size, fl->pid, fl->box);
						free(rmsg);
					}
				} break;

				case SQUIRE_VFS_SUBMESSAGE_CLOSE:{
					if(driver_info->close){
						squire_vfs_submessage_file_t * file = (squire_vfs_submessage_file_t*)(smsg_hdr+1);

						unsigned int fd = file->fdesc;
						int r = -1;

						size_t rmsg_size = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t));
						squire_vfs_message_header_t * rmsg;
						squire_vfs_submessage_file_t * fl = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&rmsg, rmsg_size, driver_info->rand_key, SQUIRE_VFS_SUBMESSAGE_CLOSE_R);

						// Check if combo fd and pid in list
						vfs_opened_file_t * o = driver_info->openfiles;
						vfs_opened_file_t * po = 0;
						while(o){
							if(o->fd==fd && o->pid==from){
								// Check if signature is correct (should be sig_d)
								// Get signature [sig_d]sig_u|nonce
								squire_chacha(o->signature, 0, o->nonce, hdr->signature, 32);
								if(!memcmp2(driver_info->rand_key, hdr->signature, 32)){
									r = driver_info->close(fd);

									// Delete opened file from list
									if(po) po->next = o->next;
									else driver_info->openfiles = 0;
									free(o);
								}
								break;
							}
							po = o;
							o->next;
						}

						memcpy(fl, file, sizeof(squire_vfs_submessage_file_t));
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
	extern char _signature[33];
	size_t msg_len = VFS_MSG_LEN(sizeof(squire_vfs_submessage_mount_t));
	squire_vfs_message_header_t * msg;
	squire_vfs_submessage_mount_t * mount = (squire_vfs_submessage_mount_t*) squire_vfs_create_message(&msg, msg_len, _signature, SQUIRE_VFS_SUBMESSAGE_MOUNT);

	mount->mountpoint = mountpoint;
	strcpy3(mount->device, device);
	strcpy3(mount->type, type);
	mount->box = 255;
	mount->pid = squire_procthread_getpid();

	squire_rpc_box(255, SQUIRE_VFS_PID, SQUIRE_VFS_USER_BOX, msg, msg_len, msg, msg_len);
	int status = mount->status;

	// TODO Check if message has correct type

	// Check if message has correct signature, else return error
	if(memcmp2(_signature, msg->signature, 32)!=0){
		status = -2;
	}

	free(msg);
	return status;
}