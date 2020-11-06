#include <squire.h>
#include <squire_driver.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

unsigned int device_manager_pid = 0;
unsigned int device_manager_box = 0;

extern SQUIRE_DRIVER_INFO * _driver_info;
extern unsigned int _driver_simple_box;

// Driver is called as 'driver.bin DevMan_PID DevMan_message_box'
int driver_main(int argc, char ** argv){

	// Get the paramters
	if(argc!=3){
		printf("Driver must be executed with 3 parameters: name DevMan_PID DevMan_box\r\n");
		return EXIT_FAILURE;
	}
	char * param_name = argv[0];
	unsigned int param_devman_pid = atoi(argv[1]);
	unsigned int param_devman_box = atoi(argv[2]);
	device_manager_pid = param_devman_pid;
	device_manager_box = param_devman_box;

	// The driver must send the driver information to the device manager
	// Calculate driver info structure size
	size_t driver_info_size = sizeof(squire_driver_t) + sizeof(squire_driver_supported_t);
	int supported_nr = 1;
	squire_driver_supported_t * supp = _driver_info->driver_info->supported;
	while(strlen(supp->id)!=0){
		driver_info_size += sizeof(squire_driver_supported_t);
		supported_nr++;
		supp += 1;
	}
	size_t message_size = driver_info_size + sizeof(squire_driver_message_t) + supported_nr*sizeof(squire_driver_submessage_t);
	squire_driver_message_t * infomsg = (squire_driver_message_t*)alloca(message_size);
	infomsg->amount_messages = 1;
	strcpy(infomsg->messages[0].name, "");
	infomsg->messages[0].type = SUBMESSAGE_TYPE_O_INFODRIVER;
	infomsg->messages[0].size = driver_info_size;
	memcpy(&infomsg->messages[0].content, _driver_info->driver_info, driver_info_size);
	int stat = squire_message_simple_box_send(infomsg, message_size, device_manager_pid, device_manager_box);

	// The main functions becomes the message handler
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, _driver_info->driver_info->simple_box);

		squire_driver_message_t * msg = (squire_driver_message_t*) msg_buffer;
		squire_driver_submessage_t * submsg = &msg->messages[0];
		for(int i=0; i<msg->amount_messages; i++){
			switch(submsg->type){
				case SUBMESSAGE_TYPE_I_FUNCTION:{
					squire_driver_submessage_function_t * func = &submsg->content;
					if(!strcmp(submsg->name, _driver_info->driver_info->name)){
						_driver_info->function_callback(from, func);
					}
				} break;
				default:
					break;
			}
			submsg = (squire_driver_submessage_t*)((void*)&submsg->content + submsg->size);
		}

	}
	return 0;
} 

