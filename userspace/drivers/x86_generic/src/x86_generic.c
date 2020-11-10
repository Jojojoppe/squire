#include <x86_generic.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

#include <x86_generic_RTC.h>

#define DEVICE_ID "x86_generic"
#define DEVICE_INSTANCE 0

unsigned int nr_connected_devices = 2;
char * connected_devices[] = {
	"x86_generic_PCI",
	"x86_generic_RTC",
};

void INTRhandler(int sig){
	unsigned int intr_id = squire_extraval0;
	if(intr_id==RTC_INTR){
		x86_generic_RTC_INTR();
	}
}

void x86_generic_function_callback(unsigned int from, squire_driver_submessage_function_t * func){
	switch(func->function){
	
		case DRIVER_FUNCTIONS_INIT: {
			printf("x86_generic %08x] Initialize device\r\n", func->instance);
			// Setup interrupt handler
			signal(SIGINTR, INTRhandler);
		} break;

		case DRIVER_FUNCTIONS_ENUM:{
			printf("x86_generic %08x] Enumerate device\r\n", func->instance);

			unsigned int nr_devices = nr_connected_devices;
			// Message size
			size_t msg_size = sizeof(squire_driver_message_t) + nr_devices*(sizeof(squire_driver_submessage_t)+sizeof(squire_driver_submessage_device_t));
			squire_driver_message_t * msg = (squire_driver_message_t*)alloca(msg_size);
			msg->amount_messages = nr_devices;
			squire_driver_submessage_t * submsg = msg->messages;
			for(int i=0; i<nr_devices; i++){
				strcpy(submsg->name, "");
				submsg->type = SUBMESSAGE_TYPE_O_REGDEVICE;
				submsg->size = sizeof(squire_driver_submessage_device_t);
				squire_driver_submessage_device_t * dev = (squire_driver_submessage_device_t*)submsg->content;
				strcpy(dev->parent, DEVICE_ID);
				dev->parent_instance = DEVICE_INSTANCE;
				dev->instance = 0;
				strcpy(dev->id, connected_devices[i]);
				submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
			}
			squire_message_simple_box_send(msg, msg_size, device_manager_pid, device_manager_box);

		}break;

		default:
			printf("x86_generic %08x] Unknown function requested\r\n", func->instance);
			break;
	}
}
