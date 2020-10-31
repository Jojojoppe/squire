#include <_PDCLIB_threadconfig.h>
#include <threads.h>
#include <squire.h>
#include <stdlib.h>

int thrd_equal(thrd_t thr0, thrd_t thrd1){
    if(thr0 == thrd1)
        return 0;
    return -1;
}
