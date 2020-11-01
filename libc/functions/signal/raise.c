#include <signal.h>
#include <squire.h>

int raise( int sig ){
    squire_procthread_kill(0, sig);
    return 0;
}