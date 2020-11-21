#include <squire.h>

#include <stddef.h>

int squire_rpc_box(unsigned int ownbox, unsigned pid, unsigned int box, void * data, size_t length, void * buffer, size_t buffer_length){
	int status = 0;

	// Send message
	status = squire_message_simple_box_send(data, length, pid, box);
	if(status)
		return status;

	// Wait for return message
	while(1){
		unsigned int from;
		size_t l = buffer_length;
		status = squire_message_simple_box_receive(buffer, &l, &from, RECEIVE_BLOCKED, ownbox);
		if(status==0 && from==pid){
			return 0;
		}else if(status){
			return status;
		}
	}

	return status;
}
