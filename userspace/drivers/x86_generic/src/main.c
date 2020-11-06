#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_driver.h>

squire_driver_t driver_x86_generic = {
	"x86_generic",
	1,0,
	DRIVER_FUNCTIONS_INIT | DRIVER_FUNCTIONS_DEINIT | DRIVER_FUNCTIONS_ENUM,
	{
		{"x86_generic_8259PIC", "x86_generic_8259PIC"},
		{"x86_generic_16650UART", "x86_generic_16650UART"},
		{"x86_generic_PCI", "x86_generic_PCI"},
		{0, 0},
	},
};

// Driver is called as 'driver.bin DevMan_PID DevMan_message_box'
int main(int argc, char ** argv){

	// On entry the driver must send the driver information to the driver manager	

	// The main functions becomes the message handler
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;
		squire_message_status_t status = squire_message_simple_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED);
		if(!status){

			// Message is received
			squire_driver_message_t * message = (squire_driver_message_t*)msg_buffer;
			squire_driver_submessage_t * submessage = &message->messages[0];
			for(int sub=0; sub<message->amount_messages; sub++){

				// Calculate address of next submessage
				submessage = (squire_driver_submessage_t*)((void*)submessage + submessage->size);
			}

		}else{
			printf("Message receive error: %d\r\n", status);
		}
	}
	return 0;
} 
