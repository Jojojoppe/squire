#include <general/message.h>
#include <general/arch/proc.h>
#include <general/string.h>
#include <general/kmalloc.h>
#include <general/schedule.h>

void message_init_info(message_info_t * info){
    info->simple = 0;
    info->simple_number = 0;
    info->simple_recv_thread = 0;
}

unsigned int message_simple_send(unsigned int to, size_t length, void * data){
    if(length<8)
        return MESSAGE_SIMPLE_ERROR_MESSAGE_TO_SHORT;

    // Get message info structure of receiver
    proc_proc_t * rec = proc_get(to);
    if(!rec)
        return MESSAGE_SIMPLE_ERROR_TO_NONEXIST;

    // Create message structure
    message_simple_t * msg = (message_simple_t*)kmalloc(sizeof(message_simple_t)+(length-8));
    msg->from = proc_proc_get_current()->id;
    msg->length = length;
    memcpy(msg->data, data, length);

    // Add message to message queue at the end
    msg->next = 0;
    if(!rec->message_info.simple_number){
        // First message
        rec->message_info.simple_number = 1;
        rec->message_info.simple = msg;
    }else{
        // Find the last one
        message_simple_t * last = rec->message_info.simple;
        message_simple_t * m = last;
        for(int i=0; i<rec->message_info.simple_number; i++){
            m = last;
            last = last->next;
        }
        m->next = msg;
        rec->message_info.simple_number++;
    }

    // Wake up thread of receiving side if needed
    if(rec->message_info.simple_recv_thread){
        proc_thread_t * thread = proc_thread_get(rec->message_info.simple_recv_thread, to);
        thread->state = PROC_TRHEAD_STATE_RUNNING;
        rec->message_info.simple_recv_thread = 0;
    }

    return MESSAGE_SIMPLE_ERROR_NOERROR;
}

unsigned int message_simple_receive(void * buffer, size_t * length, unsigned int * from){
    if(*length<8)
        return MESSAGE_SIMPLE_ERROR_BUFFER_TO_SHORT;

    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;
    if(!info->simple_number)
        return MESSAGE_SIMPLE_ERROR_NO_MESSAGES;

    message_simple_t * msg = info->simple;
    info->simple = msg->next;
    info->simple_number--;

    *from = msg->from;
    *length = msg->length;
    memcpy(buffer, msg->data, msg->length);

    return MESSAGE_SIMPLE_ERROR_NOERROR;
}

unsigned int message_simple_receive_blocking(void * buffer, size_t * length, unsigned int * from){
    if(*length<8)
        return MESSAGE_SIMPLE_ERROR_BUFFER_TO_SHORT;

    // Get own message info structure
    proc_proc_t * current = proc_proc_get_current();
    message_info_t * info = &current->message_info;
    if(info->simple_recv_thread)
        return MESSAGE_SIMPLE_ERROR_ALREADY_BLOCKED_RECEIVE;
    while(!info->simple_number){
        proc_thread_get_current()->state = PROC_THREAD_STATE_WAITING;
        info->simple_recv_thread = proc_thread_get_current()->id;
        schedule();
    }

    message_simple_t * msg = info->simple;
    info->simple = msg->next;
    info->simple_number--;

    memset(buffer, 0, *length);
    *from = msg->from;
    *length = msg->length;
    memcpy(buffer, msg->data, msg->length);

    return MESSAGE_SIMPLE_ERROR_NOERROR;
}