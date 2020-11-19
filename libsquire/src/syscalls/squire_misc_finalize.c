#include <squire_syscall.h>

void squire_misc_finalize(){
	squire_syscall_misc_t params;
	params.operation = SQUIRE_SYSCALL_MISC_OPERATION_FINALIZE;
	squire_syscall(SQUIRE_SYSCALL_MISC, sizeof(params), &params);
}
