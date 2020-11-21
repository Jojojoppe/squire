#include <general/message.h>
#include <general/arch/proc.h>
#include <general/string.h>
#include <general/kmalloc.h>
#include <general/schedule.h>

// All the message docks
message_simple_dock_t message_docks[512];

void message_init_info(message_info_t * info){
	// Init simple message
    info->simple = 0;
    info->simple_number = 0;
    info->simple_recv_thread = 0;
	// Init simple message box
	for(int i=0; i<256; i++){
		info->simple_box[i] = 0;
		info->simple_box_number[i] = 0;
		info->simple_box_recv_thread[i] = 0;
	}
}

// Internal simple message functions
unsigned int _mesage_simple_send(unsigned int to, size_t length, void * data, message_simple_t ** simple, unsigned int * number, unsigned int * thread){
	if(length<8)
        return MESSAGE_SIMPLE_ERROR_MESSAGE_TO_SHORT;

    // Create message structure
    message_simple_t * msg = (message_simple_t*)kmalloc(sizeof(message_simple_t)+(length-8));
    msg->from = proc_proc_get_current()->id;
    msg->length = length;
    memcpy(msg->data, data, length);

    // Add message to message queue at the end
    msg->next = 0;
    if(!(*number)){
        // First message
        *number = 1;
        *simple = msg;
    }else{
        // Find the last one
        message_simple_t * last = *simple;
        message_simple_t * m = last;
        for(int i=0; i<(*number); i++){
            m = last;
            last = last->next;
        }
        m->next = msg;
        (*number)++;
    }

    // Wake up thread of receiving side if needed
    if((*thread)){
        schedule_schedulable_t * s = schedule_get(to, *thread);
        if(s){
//			printf("-- waking up %d-%d\r\n", to, *thread);
            schedule_set_state(s, SCHEDULE_STATE_RUNNING);
        }else{
            printf("ERROR: pid/tid does not exist. Cannot wake up receiving thread\r\n");
        }

        *thread = 0;
    }

    return MESSAGE_SIMPLE_ERROR_NOERROR;
}

unsigned int _message_simple_receive(void * buffer, size_t * length, unsigned int * from, message_simple_t ** simple, unsigned int * number, unsigned int * thread, unsigned int blocked){
	if(*length<8)
        return MESSAGE_SIMPLE_ERROR_BUFFER_TO_SHORT;

	if(!blocked){
		if(!(*number)){
			return MESSAGE_SIMPLE_ERROR_NO_MESSAGES;
		}
	}else{
		if((*thread)){
			printf("Thread: %d\r\n", thread);
			return MESSAGE_SIMPLE_ERROR_ALREADY_BLOCKED_RECEIVE;
		}
		while(!(*number)){
			*thread = proc_thread_get_current()->id;
//			printf("-- waiting for message from %d\r\n", *thread);
			schedule_set_state(0, SCHEDULE_STATE_IDLE);
			schedule();
//			printf("-- woken up\r\n");
			*thread = 0;
		}
	}

    message_simple_t * msg = *simple;
    *simple = msg->next;
    (*number)--;

    memset(buffer, 0, *length);
    *from = msg->from;
    *length = msg->length;
    memcpy(buffer, msg->data, msg->length);

    kfree(msg);

    return MESSAGE_SIMPLE_ERROR_NOERROR;
}

unsigned int message_simple_send(unsigned int to, size_t length, void * data){
    // Get message info structure of receiver
    proc_proc_t * rec = proc_get(to);
    if(!rec){
        return MESSAGE_SIMPLE_ERROR_TO_NONEXIST;
	}
	return _mesage_simple_send(to, length, data, &rec->message_info.simple, &rec->message_info.simple_number, &rec->message_info.simple_recv_thread);
}

unsigned int message_simple_receive(void * buffer, size_t * length, unsigned int * from){
    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;
	return _message_simple_receive(buffer, length, from, &info->simple, &info->simple_number, &info->simple_recv_thread, 0); 
}

unsigned int message_simple_receive_blocking(void * buffer, size_t * length, unsigned int * from){
    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;

	return _message_simple_receive(buffer, length, from, &info->simple, &info->simple_number, &info->simple_recv_thread, 1); 
}

unsigned int message_simple_box_send(unsigned int to, size_t length, void * data, unsigned int box){
    // Get message info structure of receiver
    proc_proc_t * rec = proc_get(to);
    if(!rec){
        return MESSAGE_SIMPLE_ERROR_TO_NONEXIST;
	}
	return _mesage_simple_send(to, length, data, &rec->message_info.simple_box[box], &rec->message_info.simple_box_number[box], &rec->message_info.simple_box_recv_thread[box]);
}

unsigned int message_simple_box_receive(void * buffer, size_t * length, unsigned int * from, unsigned int box){
    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;
	return _message_simple_receive(buffer, length, from, &info->simple_box[box], &info->simple_box_number[box], &info->simple_box_recv_thread[box], 0); 
}

unsigned int message_simple_box_receive_blocking(void * buffer, size_t * length, unsigned int * from, unsigned int box){
    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;

	return _message_simple_receive(buffer, length, from, &info->simple_box[box], &info->simple_box_number[box], &info->simple_box_recv_thread[box], 1); 
}

void message_simple_debug(){
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;
    if(!info->simple_number){
        printf("No messages in queue\r\n");
        return;
    }   

    message_simple_t * msg = info->simple;
    printf("Messages:\r\n");
    while(msg){
        printf("- MSG[%08x]: from %08x %08x %08x %08x\r\n", msg, msg->from, msg->length, msg->data, msg->next);
        msg = msg->next;
    }

}
