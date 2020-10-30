#include <threads.h>

void mtx_destroy(mtx_t *mtx)
{
	squire_syscall_mutex_deinit(*mtx);
}
