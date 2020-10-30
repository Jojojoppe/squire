#include <threads.h>

int mtx_init(mtx_t *mtx, int type)
{
	*mtx = squire_syscall_mutex_init();
	return thrd_success;
}
