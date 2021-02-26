#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#include <squire.h>

#include "vfs.h"

vfs_driver_t * vfs_registered_drivers;
vfs_mountpoint_t vfs_mountpoints[256];

// Find a driver supporting an fs type
vfs_driver_t * vfs_find_driver(char * type){
    vfs_driver_t * driver = vfs_registered_drivers;
    while(driver){
        for(int i=0; i<32; i++){
            if(driver->driver_info.supported[i].type==0) break;
            if(!strcmp(driver->driver_info.supported[i].type, type)){
                return driver;
            }
        }
        driver = driver->next;
    }
    return 0;
}

// USER-VFS open
void vfs_user_open(squire_vfs_submessage_file_t * file, unsigned int from){
    // Check if mountpoint is mounted
    if(vfs_mountpoints[file->mountpoint].driver){
        vfs_driver_t * d = vfs_mountpoints[file->mountpoint].driver;
        if(d){
            // Send to driver
            size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_file_t);
            size_t mmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
            squire_vfs_message_header_t * mmsg = (squire_vfs_message_header_t*)malloc(mmsg_size);
            memset(mmsg, 0, mmsg_size);
            mmsg->length = mmsg_size;
            mmsg->messages = 1;
            squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(mmsg+1);
            smsg_header->length = smsg_size;
            smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPEN;
            squire_vfs_submessage_file_t * fl = (squire_vfs_submessage_file_t*)(smsg_header+1);
            memcpy(fl, file, sizeof(squire_vfs_submessage_file_t));
            fl->dpid = d->pid;
            fl->dbox = d->driver_info.box;
            squire_message_simple_box_send(mmsg, mmsg_size, d->pid, d->driver_info.box);
            free(mmsg);
            return;
        }
    }

    // Send error return
	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_file_t);
	size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
	memset(rmsg, 0, rmsg_size);
	rmsg->length = rmsg_size;
	rmsg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPEN_R;
	squire_vfs_submessage_file_t * d = (squire_vfs_submessage_file_t*)(smsg_header+1);
    memcpy(d, file, sizeof(squire_vfs_submessage_file_t));
    d->status = -1;
    squire_message_simple_box_send(rmsg, rmsg_size, from, d->box);
    free(rmsg);
}

// USER-VFS opendir
void vfs_user_opendir(squire_vfs_submessage_dir_t * dir, unsigned int from){
    // Check if mountpoint is mounted
    if(vfs_mountpoints[dir->mountpoint].driver){
        vfs_driver_t * d = vfs_mountpoints[dir->mountpoint].driver;
        if(d){
            // Send to driver
            size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
            size_t mmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
            squire_vfs_message_header_t * mmsg = (squire_vfs_message_header_t*)malloc(mmsg_size);
            memset(mmsg, 0, mmsg_size);
            mmsg->length = mmsg_size;
            mmsg->messages = 1;
            squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(mmsg+1);
            smsg_header->length = smsg_size;
            smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR;
            squire_vfs_submessage_dir_t * dr = (squire_vfs_submessage_dir_t*)(smsg_header+1);
            memcpy(dr, dir, sizeof(squire_vfs_submessage_dir_t));
            dr->dpid = d->pid;
            dr->dbox = d->driver_info.box;
            squire_message_simple_box_send(mmsg, mmsg_size, d->pid, d->driver_info.box);
            free(mmsg);
            return;
        }
    }

    // Send error return
	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
	size_t rmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * rmsg = (squire_vfs_message_header_t*)malloc(rmsg_size);
	memset(rmsg, 0, rmsg_size);
	rmsg->length = rmsg_size;
	rmsg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(rmsg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR_R;
	squire_vfs_submessage_dir_t * d = (squire_vfs_submessage_dir_t*)(smsg_header+1);
    memcpy(d, dir, sizeof(squire_vfs_submessage_dir_t));
    d->status = -1;
    squire_message_simple_box_send(rmsg, rmsg_size, from, d->box);
    free(rmsg);
}

// USER-VFS mount
void vfs_user_mount(squire_vfs_submessage_mount_t * msg, unsigned int from){
    // Check if mountpoint is free
    if(!vfs_mountpoints[msg->mountpoint].driver){
        // Search for usable driver
        vfs_driver_t * d = vfs_find_driver(msg->type);
        if(d){
            vfs_mountpoints[msg->mountpoint].driver = d;
            strcpy(vfs_mountpoints[msg->mountpoint].device, msg->device);

            // May mount filesystem!
            size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_mount_t);
            size_t mmsg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
            squire_vfs_message_header_t * mmsg = (squire_vfs_message_header_t*)malloc(mmsg_size);
            memset(mmsg, 0, mmsg_size);
            mmsg->length = mmsg_size;
            mmsg->messages = 1;
            squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(mmsg+1);
            smsg_header->length = smsg_size;
            smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_MOUNT;
            squire_vfs_submessage_mount_t * mount = (squire_vfs_submessage_mount_t*)(smsg_header+1);
            memcpy(mount, msg, sizeof(squire_vfs_submessage_mount_t));
            squire_message_simple_box_send(mmsg, mmsg_size, d->pid, d->driver_info.box);
            free(mmsg);
            return;
        }
    }

    // Send error return
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
    memcpy(mount, msg, sizeof(squire_vfs_submessage_mount_t));
    mount->status = -1;
    squire_message_simple_box_send(rmsg, rmsg_size, from, mount->box);
    free(rmsg);
}

// VFS-DRIVER mount return message
void vfs_driver_mount_r(squire_vfs_submessage_mount_t * msg, unsigned int from){
    int status = msg->status;

    // Check for errors
    if(msg->status){
        vfs_mountpoints[msg->mountpoint].driver = 0;
        status = -1;
        printf("VFS-DRIVER] could not mount to mp%d\r\n", msg->mountpoint);
    }

    // Send error return
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
    memcpy(mount, msg, sizeof(squire_vfs_submessage_mount_t));
    mount->status = status;
    squire_message_simple_box_send(rmsg, rmsg_size, mount->pid, mount->box);
    free(rmsg);
}

// VFS-DRIVER register a driver
void vfs_driver_register_driver(squire_vfs_driver_t * driver_info, unsigned int from){
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
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, SQUIRE_VFS_USER_BOX);

        squire_vfs_message_header_t * hdr = (squire_vfs_message_header_t*) msg_buffer;
        squire_vfs_submessage_header_t * smsg_hdr = (squire_vfs_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                case SQUIRE_VFS_SUBMESSAGE_MOUNT:
                    vfs_user_mount(smsg_hdr+1, from);
                    break;

                case SQUIRE_VFS_SUBMESSAGE_OPENDIR:
                    vfs_user_opendir(smsg_hdr+1, from);
                    break;

                case SQUIRE_VFS_SUBMESSAGE_OPEN:
                    vfs_user_open(smsg_hdr+1, from);
                    break;

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
    thrd_t thrd_public;
    thrd_create(&thrd_public, vfs_user_main, 0);

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

                case SQUIRE_VFS_SUBMESSAGE_MOUNT_R:
                    vfs_driver_mount_r(smsg_hdr+1, from);
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