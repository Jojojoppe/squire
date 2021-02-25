#include <squire.h>
#include <squire_rpc.h>
#include <squire_vfs.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <threads.h>

extern squire_vfs_driver_t * __vfs_driver_info;

int squire_vfs_driver_main(int argc, char ** argv){

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
    __vfs_driver_info->pid = squire_procthread_getpid();
    memcpy(smsg_content, __vfs_driver_info, sizeof(squire_vfs_driver_t));
    squire_message_simple_box_send(msg, message_size, SQUIRE_VFS_PID, SQUIRE_VFS_DRIVER_BOX);
    free(msg);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, __vfs_driver_info->box);

        squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg_buffer;
        squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                default:
                    break;
            }
            smsg_hdr = (squire_vfs_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        };
	}
	return EXIT_SUCCESS;
}