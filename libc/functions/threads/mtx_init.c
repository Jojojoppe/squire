#include <threads.h>

int mtx_init(mtx_t *mtx, int type)
{
	*mtx = squire_mutex_create();
	return thrd_success;
}
