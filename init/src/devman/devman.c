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

devman_device_t * _0_find_device(devman_device_t * device, const char * id){
	while(device){
		// Recursive enter one level down into childs
		devman_device_t * tmp = _0_find_device(device->childs, id);
		if(tmp){
			return tmp;
		}

		if(!strcmp(device->id, id)){
			return device;
		}

		device = device->next;
	}
	return 0;
}
devman_device_t * find_device(const char * id){
	return _0_find_device(device_tree, id);
}

void _0_init_unitialized(devman_device_t * device, devman_driver_t * driver){
	while(device){

		// Recursive enter one level down into childs
		_0_init_unitialized(device->childs, driver);

		// Check if need to be initialized and it can be done by this driver
		if(device->driver==0 && strcmp(driver->id, device->id)==0){

			device->functions = driver->functions;
			device->driver = driver;

			// Send init and enum request if supported
			size_t msglen = 1;
			uint8_t nr = 0;
			if((device->functions&DRIVER_FUNCTIONS_INIT)==DRIVER_FUNCTIONS_INIT){
				msglen += sizeof(squire_driver_submessage_t) + sizeof(squire_driver_submessage_function_t);
				nr++;
			}
			if((device->functions&DRIVER_FUNCTIONS_ENUM)==DRIVER_FUNCTIONS_ENUM){
				msglen += sizeof(squire_driver_submessage_t) + sizeof(squire_driver_submessage_function_t);
				nr++;
			}
			squire_driver_message_t * request = alloca(msglen);
			request->amount_messages = nr;
			squire_driver_submessage_t * submsg = &request->messages[0];
			if((device->functions&DRIVER_FUNCTIONS_INIT)==DRIVER_FUNCTIONS_INIT){
				strcpy(submsg->name, driver->name);
				submsg->type = SUBMESSAGE_TYPE_I_FUNCTION;
				submsg->size = sizeof(squire_driver_submessage_function_t);
				squire_driver_submessage_function_t * func = submsg->content;
				func->function = DRIVER_FUNCTIONS_INIT;
				func->instance = device->instance;
				strcpy(func->id, device->id);
				submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
			}
			if((device->functions&DRIVER_FUNCTIONS_ENUM)==DRIVER_FUNCTIONS_ENUM){
				strcpy(submsg->name, driver->name);
				submsg->type = SUBMESSAGE_TYPE_I_FUNCTION;
				submsg->size = sizeof(squire_driver_submessage_function_t);
				squire_driver_submessage_function_t * func = submsg->content;
				func->function = DRIVER_FUNCTIONS_ENUM;
				func->instance = device->instance;
				strcpy(func->id, device->id);
				submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
			}
			int status = -1;
			if(nr)
				status = squire_message_simple_box_send(request, msglen, driver->PID, driver->simple_box);
		}

		device = device->next;
	}
}
void init_unititialized(devman_driver_t * driver){
	_0_init_unitialized(device_tree, driver);
}

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

		// Traverse device tree to initialize devices which can be served by this driver
		init_unititialized(driver);

		supp += 1;
	}
}

void recv_regdevice(unsigned int from, squire_driver_submessage_device_t * device){
	devman_device_t * parent = find_device(device->parent);
	if(!parent){
		printf("Parent not found, rogue device...\r\n");
		return;
	}

	// Create new device structure
	devman_device_t * new = (devman_device_t*)malloc(sizeof(devman_device_t));
	strcpy(new->id, device->id);
	new->instance = device->instance;
	new->childs = 0;
	new->next = 0;
	new->functions = 0;
	new->driver = 0;

	// Link to parent
	if(parent->childs){
		devman_device_t * c = parent->childs;
		while(c->next) c = c->next;
		c->next = new;
	}else{
		parent->childs = new;
	}

	printf("Added device '%s-%08x' as child of '%s'\r\n", new->id, new->instance, parent->id);

	// Check if there is already a driver for this device loaded
	devman_driver_t * driver = driver_list;
	while(driver){
		init_unititialized(driver);
		driver = driver->next;
	}
	// TODO check if still not initalized.. what to do then?
	if(!new->driver){
		printf("No driver found for device\r\n");
	}else{
		printf("Driver '%s' found for device\r\n", new->driver->name);
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
	device_tree->instance = 0;
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
				case SUBMESSAGE_TYPE_O_REGDEVICE:
					recv_regdevice(from, submsg->content);
					break;
				default:
					break;
			}
			submsg = (squire_driver_submessage_t*)((void*)submsg + submsg->size + 72);
		}

	}
	return EXIT_SUCCESS;
}
