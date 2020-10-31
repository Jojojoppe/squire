#include <stdlib.h>

#include "_PDCLIB_glue.h"
#include <squire.h>

void _PDCLIB_Exit(int status){
    squire_syscall_exit(status);
}