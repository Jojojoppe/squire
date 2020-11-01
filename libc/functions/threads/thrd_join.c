#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

int thrd_join(thrd_t thr, int * res){
    int retval = squire_procthread_join(thr);
    if(res)
        *res = retval;
    return thrd_success;
}