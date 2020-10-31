#include <signal.h>
#include <squire.h>
#include <stdio.h>
#include <errno.h>

static unsigned int _PDCLIB_signal_initialized;

static void (*_PDCLIB_sighandlers[9])(int);

void _PDCLIB_sighandler(int sig){
    int signumber = sig;
    if(sig>7){
        // Raised signal not a valid signal, take general sighandler
        sig = 8;
    }
    // Check if sighandler is installed and if it may be handled
    if(_PDCLIB_sighandlers[sig] && sig!=SIGKILL){
        // Call sighandler
        _PDCLIB_sighandlers[sig](signumber);

        return;
    }
    // Default signal handlers
    char * message;
    switch(sig){
        case SIGTERM:
            message = "Termination request (SIGTERM)\r\n";
            break;
        case SIGKILL:
            message = "Process killed (SIGKILL)\r\n";
            break;
        case SIGABRT:
            message = "Abnormal termination (SIGABRT)\r\n";
            break;
        case SIGFPE:
            message = "Arithmetic exception (SIGFPE)\r\n";
            break;
        case SIGILL:
            message = "Illegal instruction (SIGILL)\r\n";
            break;
        case SIGSEGV:
            message = "Segmentation error (SIGSEGV)\r\n";
            break;
        case SIGETC:
            message = "Unknown kernel interrupt (SITETC)\r\n";
            break;
        case SIGINT:
            message = "Unhandled software interrupt (SIGINT)\r\n";
            break;

        case _SIGUNDEF:
        default:
            return;
    }
    fputs(message, stderr);
    exit(sig);
}

void _PDCLIB_signal_initialize(){
    // Register overall signal handler
    _PDCLIB_signal_initialized = 1;

    for(int i=0; i<8; i++){
        _PDCLIB_sighandlers[i] = SIG_DFL;
    }

    squire_syscall_signal(_PDCLIB_sighandler);
}

void (*signal( int sig, void (*func)( int ) ) )( int ){
    if(!_PDCLIB_signal_initialized){
        _PDCLIB_signal_initialize();
    }

    if(sig>7){
        // sig not a valid signal, take general sighandler
        sig = 8;
    }

    void (*oldhandler)(int) = _PDCLIB_sighandlers[sig];
    // set new handler
    _PDCLIB_sighandlers[sig] = func;

    return oldhandler;
}