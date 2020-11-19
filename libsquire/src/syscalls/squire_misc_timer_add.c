#include <squire_syscall.h>

void squire_misc_timer_add(unsigned long long time, unsigned int id){
    squire_syscall_misc_t params;
    params.operation = SQUIRE_SYSCALL_MISC_OPERATION_TIMER_ADD;
    params.longvalue0 = time;
    params.value0 = id;
    squire_syscall(SQUIRE_SYSCALL_MISC, sizeof(params), &params);
}