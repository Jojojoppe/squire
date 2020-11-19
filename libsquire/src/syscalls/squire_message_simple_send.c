#include <squire_syscall.h>

squire_message_status_t squire_message_simple_send(void * buffer, size_t length, unsigned int to){
    squire_syscall_message_t params;
    params.operation = SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_SEND;
    params.data0 = buffer;
    params.length0 = length;
    params.to0 = to;
    squire_syscall(SQUIRE_SYSCALL_MESSAGE, sizeof(params), &params);
    return (squire_message_status_t)params.return0;
}