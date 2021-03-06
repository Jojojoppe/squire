#include <squire_syscall.h>

squire_message_status_t squire_message_simple_receive(void * buffer, size_t * length, unsigned int * from, int flags){
    squire_syscall_message_t params;
    params.operation = SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_RECEIVE;
    params.data0 = buffer;
    params.length0 = *length;
    params.flags = flags;
    squire_syscall(SQUIRE_SYSCALL_MESSAGE, sizeof(params), &params);
    *length = params.length0;
    *from = params.from0;
    return params.return0;
}