#include <threads.h>

int mtx_trylock(mtx_t *mtx)
{
	unsigned int status = squire_syscall_mutex_status(*mtx);
	if(status)
		return thrd_busy;
	else
		return thrd_success;
}
