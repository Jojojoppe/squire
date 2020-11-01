#include <threads.h>

void mtx_destroy(mtx_t *mtx)
{
	squire_mutex_destroy(*mtx);
}
