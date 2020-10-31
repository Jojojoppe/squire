#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

thrd_t thrd_current(){
    unsigned int pid, tid;
    squire_syscall_getid(&pid, &tid);
    thrd_t t;
    t = tid;
    return t;
}