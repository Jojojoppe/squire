#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_driver.h>

#include "tar.h"

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	// Load x86_generic driver
	void * tar_start = (void*)(*((unsigned int*)argv[1]));
	unsigned int length;
	void * x86_generic = tar_get(tar_start, "x86_generic.bin", &length);
	char * x86_generic_argv0 = "x86_generic.bin";
	char ** x86_generic_argv[2];
	x86_generic_argv[0] = x86_generic_argv0;
	unsigned int x86_generic_PID = squire_procthread_create_process(x86_generic, length, 2, x86_generic_argv);

	// Send INTI and ENUM request to x86_generic
	squire_driver_message_t MSG = {
		1, {
			{sizeof(squire_driver_submessage_t), "x86_generic", SUBMESSAGE_TYPE_I_FUNCTION, {
				DRIVER_FUNCTIONS_INIT, "", 0, "",
			}},
		},
	};
	int s = squire_message_simple_send(&MSG, sizeof(squire_driver_message_t) + MSG.amount_messages*sizeof(squire_driver_submessage_t), x86_generic_PID);

	// main becomes message handler
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED);
	}
	return 0;
} 
