#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#include <squire.h>

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

// Traverse device tree to initialize devices which can be controlled by a driver
void init_uninitialized(ddm_driver_t * driver, ddm_device_t * device){
    while(device){
        // Recursice enter one level down into childs
        init_uninitialized(driver, device->children);

        // Check if device needs to be initialized and it can be done by this driver
        if(device->driver==0){
            for(int i=0; i<32; i++){
                if(driver->driver_info.supported[i].type==0) break;
                if(!strcmp(device->type, driver->driver_info.supported[i].type)){
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

    printf("Registered device '%s' of type %s\r\n", dev->id, dev->type);
    // Check if there is a driver loaded suited for this device
    ddm_driver_t * driver = ddm_registerd_drivers;
    while(driver){
        init_uninitialized(driver, ddm_registerd_devices);
        driver = driver->next;
    }
    if(!dev->driver){
        printf("No driver found for device\r\n");
    }else{
        printf("Driver '%s' v%d.%d found for device\r\n", dev->driver->driver_info.name, dev->driver->driver_info.version_major, dev->driver->driver_info.version_minor);
    }
}

// DDM-DRIVER register a driver
void ddm_driver_register_driver(void * msg, unsigned int from){
    squire_ddm_driver_t * driver_info = (squire_ddm_driver_t*) msg;
    ddm_driver_t * d = (ddm_driver_t*) malloc(sizeof(ddm_driver_t));
    d->next = 0;
    d->pid = from;
    memcpy(&d->driver_info, driver_info, sizeof(squire_ddm_driver_t));

    if(ddm_registerd_drivers){
        ddm_driver_t * l = ddm_registerd_drivers;
        while(l->next) l=l->next;
        l->next = d;
    }else{
        ddm_registerd_drivers = d;
    }

    printf("Registered driver '%s' v%d.%d on %d:%d[:%d]\r\n", d->driver_info.name, d->driver_info.version_major, d->driver_info.version_minor, d->pid, d->driver_info.box, d->driver_info.child_box);
    for(int i=0; i<32; i++){
        if(d->driver_info.supported[i].type[0]==0) break;
        printf("    * %s\r\n", d->driver_info.supported[i].type);
    }

    init_uninitialized(d, ddm_registerd_devices);
}

/*
 * Public DDM interface
 */
int ddm_user_main(void * p){
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, SQUIRE_DDM_USER_BOX);

        squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg_buffer;
        squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                default:
                    printf("DDS-USER] Unknown submessage type %08x\r\n", smsg_hdr->submessage_type);
                    break;
            }
            smsg_hdr = (squire_ddm_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        };
        memset(msg_buffer, 0, length);
	}
	return EXIT_SUCCESS;
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

                default:
                    printf("DDS-DRIVER] Unknown submessage type %08x\r\n", smsg_hdr->submessage_type);
                    break;
            }
            smsg_hdr = (squire_ddm_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        }
	}
	return EXIT_SUCCESS;
}