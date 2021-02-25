#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#include <squire.h>

#include "vfs.h"

vfs_driver_t * vfs_registered_drivers;

// VFS-DRIVER register a driver
void vfs_driver_register_driver(void * msg, unsigned int from){
    squire_vfs_driver_t * driver_info = (squire_vfs_driver_t*) msg;
    vfs_driver_t * d = (vfs_driver_t*) malloc(sizeof(vfs_driver_t));
    d->next = 0;
    d->pid = from;
    memcpy(&d->driver_info, driver_info, sizeof(squire_vfs_driver_t));

    if(vfs_registered_drivers){
        vfs_driver_t * l = vfs_registered_drivers;
        while(l->next) l=l->next;
        l->next = d;
    }else{
        vfs_registered_drivers = d;
    }

    printf("VFS] Registered driver '%s' v%d.%d on %d:%d\r\n", d->driver_info.name, d->driver_info.version_major, d->driver_info.version_minor, d->driver_info.pid, d->driver_info.box);
    for(int i=0; i<32; i++){
        if(d->driver_info.supported[i].type[0]==0) break;
        printf("        * %s\r\n", d->driver_info.supported[i].type);
    }
};

/*
 * Public VFS interface
 */
int vfs_user_main(void * p){
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, SQUIRE_VFS_DRIVER_BOX);

        squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg_buffer;
        squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                default:
                    printf("VFS-USER] Unknown submessage type %08x\r\n", smsg_hdr->submessage_type);
                    break;
            }
            smsg_hdr = (squire_vfs_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        }
	}
	return EXIT_SUCCESS;
}

/*
 * VFS-DRIVER interface
 */
int vfs_main(void * p){
    // Start public interface
    // thrd_t thrd_public;
    // thrd_create(&thrd_public, vfs_user_main, 0);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, SQUIRE_VFS_DRIVER_BOX);

        squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg_buffer;
        squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                case SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER:
                    vfs_driver_register_driver(smsg_hdr+1, from);
                    break;

                default:
                    printf("VFS-DRIVER] Unknown submessage type %08x\r\n", smsg_hdr->submessage_type);
                    break;
            }
            smsg_hdr = (squire_vfs_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        }
	}
	return EXIT_SUCCESS;
}