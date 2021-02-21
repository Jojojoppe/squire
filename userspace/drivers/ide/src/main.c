#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_driver.h>

void function_callback(unsigned int from, squire_driver_submessage_function_t * func){
	switch(func->function){
	
		case DRIVER_FUNCTIONS_INIT: {
			printf("ide %08x] Initialize device\r\n", func->instance);
		} break;

		case DRIVER_FUNCTIONS_ENUM:{
			printf("ide %08x] Enumerate device\r\n", func->instance);

			// unsigned int nr_devices = nr_connected_devices;
			// // Message size
			// size_t msg_size = sizeof(squire_driver_message_t) + nr_devices*(sizeof(squire_driver_submessage_t)+sizeof(squire_driver_submessage_device_t));
			// squire_driver_message_t * msg = (squire_driver_message_t*)alloca(msg_size);
			// msg->amount_messages = nr_devices;
			// squire_driver_submessage_t * submsg = msg->messages;
			// for(int i=0; i<nr_devices; i++){
			// 	strcpy(submsg->name, "");
			// 	submsg->type = SUBMESSAGE_TYPE_O_REGDEVICE;
			// 	submsg->size = sizeof(squire_driver_submessage_device_t);
			// 	squire_driver_submessage_device_t * dev = (squire_driver_submessage_device_t*)submsg->content;
			// 	strcpy(dev->parent, DEVICE_ID);
			// 	dev->parent_instance = DEVICE_INSTANCE;
			// 	dev->instance = 0;
			// 	strcpy(dev->id, connected_devices[i]);
			// 	submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
			// }
			// squire_message_simple_box_send(msg, msg_size, device_manager_pid, device_manager_box);

		}break;

		default:
			printf("ide %08x] Unknown function requested\r\n", func->instance);
			break;
	}
}

squire_driver_t driver_ide = {
	"ide",																			// Name of the driver
	1,0,																			// Version of the driver
	0,																				// Simple message box the driver listens to
	{
		{"PCI_SLKDJLAKSD", DRIVER_FUNCTIONS_INIT|DRIVER_FUNCTIONS_ENUM|DRIVER_FUNCTIONS_DEINIT},
		{0},
	}																				// Supported device ID's
};
SQUIRE_DRIVER_INFO driver_info = {
	&driver_ide,
	function_callback,
};
DRIVER(driver_info)
