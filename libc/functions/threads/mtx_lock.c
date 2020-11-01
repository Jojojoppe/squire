#include <threads.h>

int mtx_lock(mtx_t *mtx)
{
	squire_mutex_lock(*mtx);
	return thrd_success;
}
