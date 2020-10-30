#include <threads.h>

int mtx_unlock(mtx_t *mtx)
{
	squire_syscall_mutex_unlock(*mtx);
    return thrd_success;
}
