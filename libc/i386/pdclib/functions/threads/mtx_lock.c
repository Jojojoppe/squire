#include <threads.h>

int mtx_lock(mtx_t *mtx)
{
	squire_syscall_mutex_lock(*mtx);
	return thrd_success;
}
