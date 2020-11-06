#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>

#include <squire.h>
#include <squire_driver.h>

#include "devman.h"

devman_device_t * device_tree;
devman_driver_t * driver_list;

void recv_infodriver(unsigned int from, squire_driver_submessage_infodriver_t * infodriver){
	// Loop over all supported devices and add to driver list
	squire_driver_supported_t * supp = &infodriver->driverinfo.supported[0];
	while(strlen(supp->id)!=0){
		devman_driver_t * driver = (devman_driver_t*)malloc(sizeof(devman_driver_t));
		driver->next = 0;
		strcpy(driver->name, infodriver->driverinfo.name);
		strcpy(driver->id, supp->id);
		driver->PID = from;
		driver->simple_box = infodriver->driverinfo.simple_box;
		driver->functions = supp->functions;

		printf("Added driver definition for '%s' with driver '%s' on %d-%d with functions %08x\r\n", driver->id, driver->name, driver->PID, driver->simple_box, driver->functions);

		if(driver_list){
			devman_driver_t * l = driver_list;
			while(l->next) l=l->next;
			l->next = driver;
		}else{
			driver_list = driver;
		}

		supp += 1;
	}
}

int devman_main(void * p){
	char * root_device = (char *)p;
	printf("Root device %s\r\n", root_device);

	// Add root device to device tree
	device_tree = (devman_device_t*)malloc(sizeof(devman_device_t));
	device_tree->childs = 0;
	device_tree->next = 0;
	strcpy(device_tree->id, root_device);
	device_tree->functions = 0;
	device_tree->driver = 0;

	driver_list = 0;

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, MSG_BOX_DEVMAN_SIMPLE);

		squire_driver_message_t * msg = (squire_driver_message_t*) msg_buffer;
		squire_driver_submessage_t * submsg = &msg->messages[0];
		for(int i=0; i<msg->amount_messages; i++){
			switch(submsg->type){
				case SUBMESSAGE_TYPE_O_INFODRIVER:
					recv_infodriver(from, submsg->content);
					break;
				default:
					break;
			}
			submsg = (squire_driver_submessage_t*)((void*)submsg + submsg->size + 72);
		}

	}
	return EXIT_SUCCESS;
}
