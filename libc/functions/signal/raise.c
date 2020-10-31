#include <signal.h>
#include <squire.h>

int raise( int sig ){
    squire_syscall_kill(0, sig);
    return 0;
}