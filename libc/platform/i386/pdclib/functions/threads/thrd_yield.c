#include <threads.h>

void thrd_yield(void)
{
	squire_syscall_yield();
}
