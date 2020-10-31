#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

void thrd_exit(int res){
    squire_syscall_exit(res);
}
