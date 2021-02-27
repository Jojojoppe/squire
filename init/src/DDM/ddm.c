#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#include <squire.h>
#include <squire_vfs.h>

#include "ddm.h"

ddm_driver_t * ddm_registerd_drivers;
ddm_device_t * ddm_registerd_devices;

// Find a device in the device tree
ddm_device_t * find_device(char * device, ddm_device_t * node){
    while(node){
        ddm_device_t * tmp = find_device(device, node->children);
        if(tmp) return tmp;
        if(!strcmp(node->id, device)) return node;
        node = node->next;
    }
    return 0;
}

// Find a driver supporting a type
ddm_driver_t * find_driver(char * type, unsigned int major, unsigned int minor, ddm_driver_t * driver){
    while(driver){
        for(int i=0; i<32; i++){
            if(driver->driver_info.supported[i].type==0) break;
            if(strcmp(driver->driver_info.supported[i].type, type)==0 && major==driver->driver_info.version_major && minor<=driver->driver_info.version_minor){
                return driver;
            }
        }
        driver = driver->next;
    }
    return 0;
}

// Traverse device tree to initialize devices which can be controlled by a driver
void init_uninitialized(ddm_driver_t * driver, ddm_device_t * device){
    while(device){
        // Recursice enter one level down into childs
        init_uninitialized(driver, device->children);

        // Check if device needs to be initialized and it can be done by this driver
        if(device->driver==0){
            char * split = strchr(device->type, '|');
            for(int i=0; i<32; i++){
                if(driver->driver_info.supported[i].type==0) break;

                // Split is used to allow for generic drivers or device specific drivers
                int suitable = 0;
                if(split){
                    char type1[64] = {0};
                    memcpy(type1, device->type, split-device->type);
                    suitable = (strcmp(type1, driver->driver_info.supported[i].type)==0) || (strcmp(split+1, driver->driver_info.supported[i].type)==0);
                }else{
                    suitable = (strcmp(device->type, driver->driver_info.supported[i].type)==0);
                }

                if(suitable){
                    // Send init and enum request
                    size_t smsg_init_length = sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_init_t);
                    size_t smsg_enum_length = sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_enum_t);
                    size_t msg_length = sizeof(squire_ddm_message_header_t) + smsg_init_length + smsg_enum_length;
                    squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) malloc(msg_length);
                    hdr->length = msg_length;
                    hdr->messages = 2;
                    squire_ddm_submessage_header_t * smsg_init_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
                    smsg_init_hdr->length = smsg_init_length;
                    smsg_init_hdr->submessage_type = SQUIRE_DDM_SUBMESSAGE_INIT;
                    squire_ddm_submessage_init_t * smsg_init = (squire_ddm_submessage_init_t*)(smsg_init_hdr+1);
                    squire_ddm_submessage_header_t * smsg_enum_hdr = (squire_ddm_submessage_header_t*)(smsg_init+1);
                    smsg_enum_hdr->length = smsg_enum_length;
                    smsg_enum_hdr->submessage_type = SQUIRE_DDM_SUBMESSAGE_ENUM;
                    squire_ddm_submessage_enum_t * smsg_enum = (squire_ddm_submessage_enum_t*)(smsg_enum_hdr+1);

                    strcpy(smsg_init->device, device->id);
                    strcpy(smsg_init->type, device->type);
                    smsg_init->parent_pid = 0;      // TODO fill in
                    smsg_init->parent_box = 0;      // TODO fill in

                    strcpy(smsg_enum->device, device->id);
                    strcpy(smsg_enum->type, device->type);

                    squire_message_simple_box_send(hdr, msg_length, driver->pid, driver->driver_info.box);
                    free(hdr);

                    device->driver = driver;
                }
            }
        }

        device = device->next;
    }
}

// DDM-DRIVER request parent driver
void ddm_driver_request_parent(void * msg, unsigned int from){
    squire_ddm_submessage_request_parent_t * m = (squire_ddm_submessage_request_parent_t*)msg;
    ddm_driver_t * d = ddm_registerd_drivers;
    while(d){
        if(!strcmp(d->driver_info.name, m->driver)){
            size_t contentsize = sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_driver_t);
            size_t retmsgsize = sizeof(squire_ddm_message_header_t) + contentsize;
            squire_ddm_message_header_t * retmsg = (squire_ddm_message_header_t*)malloc(retmsgsize);
            retmsg->length = retmsgsize;
            retmsg->messages = 1;
            squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(retmsg+1);
            smsg_hdr->length = contentsize;
            smsg_hdr->submessage_type = SQUIRE_DDM_SUBMESSAGE_PARENT;
            memcpy(smsg_hdr+1, &d->parent->driver_info, sizeof(squire_ddm_driver_t));
            squire_message_simple_box_send(retmsg, retmsgsize, from, m->box);
            free(retmsg);
            return;
        };
        d = d->next;
    }
    // ERROR
}

// DDM-DRIVER register a device
void ddm_driver_register_device(void * msg, unsigned int from){
    squire_ddm_submessage_register_device_t * m = (squire_ddm_submessage_register_device_t*) msg;
    ddm_device_t * parent = find_device(m->parent, ddm_registerd_devices);
    if(!parent){
        printf("Rogue device: parent not found...\r\n");
        return;
    }

    ddm_device_t * dev = (ddm_device_t*)malloc(sizeof(ddm_device_t));
    dev->children = 0;
    dev->next = 0;
    dev->driver = 0;
    dev->device_type = m->device_type;
    strcpy(dev->id, m->device);
    strcpy(dev->type, m->type);

    if(parent->children){
        ddm_device_t * l = parent->children;
        while(l->next) l=l->next;
        l->next = dev;
    }else{
        parent->children = dev;
    }

    printf("DDM] Registered device '%s' of type %s\r\n", dev->id, dev->type);
    // Check if there is a driver loaded suited for this device
    ddm_driver_t * driver = ddm_registerd_drivers;
    while(driver){
        init_uninitialized(driver, ddm_registerd_devices);
        driver = driver->next;
    }
    if(!dev->driver){
        // printf("  No driver found for device\r\n");
    }else{
        // printf("  Driver '%s' v%d.%d found for device\r\n", dev->driver->driver_info.name, dev->driver->driver_info.version_major, dev->driver->driver_info.version_minor);
    }
}

// DDM-DRIVER register a driver
void ddm_driver_register_driver(void * msg, unsigned int from){
    squire_ddm_driver_t * driver_info = (squire_ddm_driver_t*) msg;
    ddm_driver_t * d = (ddm_driver_t*) malloc(sizeof(ddm_driver_t));
    d->next = 0;
    d->pid = from;
    d->parent = 0;
    memcpy(&d->driver_info, driver_info, sizeof(squire_ddm_driver_t));

    if(ddm_registerd_drivers){
        ddm_driver_t * l = ddm_registerd_drivers;
        while(l->next) l=l->next;
        l->next = d;
    }else{
        ddm_registerd_drivers = d;
    }

    printf("DDM] Registered driver '%s' v%d.%d on %d:%d[:%d]\r\n", d->driver_info.name, d->driver_info.version_major, d->driver_info.version_minor, d->pid, d->driver_info.box, d->driver_info.child_box);
    for(int i=0; i<32; i++){
        if(d->driver_info.supported[i].type[0]==0) break;
        printf("         * %s\r\n", d->driver_info.supported[i].type);
    }

    // Find parent driver if needed
    if(d->driver_info.parent_name[0]){
        printf("         Needs a driver supporting %s with version > v%d.%d\r\n", d->driver_info.parent_name, d->driver_info.parent_version_major, d->driver_info.parent_version_minor);
        ddm_driver_t * p = find_driver(d->driver_info.parent_name, d->driver_info.parent_version_major, d->driver_info.parent_version_minor, ddm_registerd_drivers);
        if(p){
            d->parent = p;
            printf("         found driver '%s' v%d.%d\r\n", p->driver_info.name, p->driver_info.version_major, p->driver_info.version_minor);
        }else{
            printf("         no parent driver found\r\n");
        }
    }

    init_uninitialized(d, ddm_registerd_devices);
}

/*
 * Public DDM interface
 * user interface is a file structure inside the VFS (mp0)
 */
int ddm_fs_mount(char * type, char * device, unsigned int mountpoint, unsigned int flags){
    return 0;
}
int ddm_fs_opendir(char * path, struct dirent * dirent){
    dirent->d_fileno = 0;
    return 0;
}
ddm_device_t * _traverse_ddm_devices_int(ddm_device_t * d, unsigned int current_entry, unsigned int i, char * np){
    char * n = np;
    while(d){
        strcpy(n, d->id);
        char * nn = n + strlen(d->id);
        if(i==current_entry){
            return d;
        }
        *(nn++) = '/';
        i++;
        ddm_device_t * r = _traverse_ddm_devices_int(d->children, current_entry, i, nn);
        if(r) return r;

        d = d->next;
    }
    return NULL;
}
int ddm_fs_readdir(unsigned int current_entry, struct dirent * dirent){
    dirent->d_fileno = current_entry;

    unsigned int i=0;
    char * n = dirent->d_name;
    ddm_device_t * d = _traverse_ddm_devices_int(ddm_registerd_devices, current_entry, 0, n);
    if(d) return 0;
    return -1;
}
squire_vfs_driver_t ddmfs_driver_info = {
    "DDM_FS", 1, 0,
    0, SQUIRE_DDM_USER_BOX,
    ddm_fs_mount,
    NULL,
    ddm_fs_opendir,
    ddm_fs_readdir,
    NULL,
    NULL,
    NULL,
    {
        {"DDM_FS"}
    }
};
squire_vfs_driver_t * __vfs_driver_info = &ddmfs_driver_info;   // Must exists or else linking wouldnt work
int ddm_user_main(void * p){
    squire_vfs_driver_main_direct(0, 0, &ddmfs_driver_info);
    for(;;);
    return 0;
}

/*
 * DDM-DRIVER interface
 */
int ddm_main(void * p){

    // Create root of device tree
    ddm_device_t * rootdev = (ddm_device_t*)malloc(sizeof(ddm_device_t));
    rootdev->children = 0;
    rootdev->next = 0;
    rootdev->driver = 0;
    rootdev->device_type = SQUIRE_DDM_DEVICE_TYPE_NONE;
    ddm_registerd_devices = rootdev;
    strcpy(rootdev->id, "_");
    strcpy(rootdev->type, "ROOT");

    // Start public interface
    thrd_t thrd_public;
    thrd_create(&thrd_public, ddm_user_main, 0);

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, SQUIRE_DDM_DRIVER_BOX);

        squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg_buffer;
        squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                case SQUIRE_DDM_SUBMESSAGE_REGISTER_DRIVER:
                    ddm_driver_register_driver(smsg_hdr+1, from);
                    break;

                case SQUIRE_DDM_SUBMESSAGE_REGISTER_DEVICE:
                    ddm_driver_register_device(smsg_hdr+1, from);
                    break;

                case SQUIRE_DDM_SUBMESSAGE_REQUEST_PARENT:
                    ddm_driver_request_parent(smsg_hdr+1, from);
                    break;

                default:
                    printf("DDS-DRIVER] Unknown submessage type %08x\r\n", smsg_hdr->submessage_type);
                    break;
            }
            smsg_hdr = (squire_ddm_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        }
	}
	return EXIT_SUCCESS;
}