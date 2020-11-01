#include <general/syscall_message.h>
#include <general/syscall.h>

#include <general/kprintf.h>
#include <general/message.h>

int syscall_message(squire_syscall_message_t * params){
    switch(params->operation){

        case SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_SEND:{
            params->return0 = message_simple_send(params->to0, params->length0, params->data0);
        } break;

        case SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_RECEIVE:{
            if((params->flags&RECEIVE_BLOCKED))
                params->return0 = message_simple_receive_blocking(params->data0, &params->length0, &params->from0);
            else
                params->return0 = message_simple_receive(params->data0, &params->length0, &params->from0);
        } break;

        default:
            return SYSCALL_ERROR_OPERATION;
    }
    return SYSCALL_ERROR_NOERROR;
}