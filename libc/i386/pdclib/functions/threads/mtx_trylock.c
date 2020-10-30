#include <threads.h>

int mtx_trylock(mtx_t *mtx)
{
	squire_syscall_log("mtx_trlk\r\n", 10);
	return thrd_success;
}
