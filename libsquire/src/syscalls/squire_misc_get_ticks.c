#include <squire_syscall.h>

unsigned long long squire_misc_get_ticks(){
	squire_syscall_misc_t params;
	params.operation = SQUIRE_SYSCALL_MISC_OPERATION_GET_TICKS;
	squire_syscall(SQUIRE_SYSCALL_MISC, sizeof(params), &params);
    return params.longvalue0;
}