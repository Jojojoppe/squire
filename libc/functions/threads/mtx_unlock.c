#include <threads.h>

int mtx_unlock(mtx_t *mtx)
{
	squire_mutex_unlock(*mtx);
    return thrd_success;
}
