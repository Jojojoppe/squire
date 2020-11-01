#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

thrd_t thrd_current(){
    thrd_t t = squire_procthread_gettid();
    return t;
}