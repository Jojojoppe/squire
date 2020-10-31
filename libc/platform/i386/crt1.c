#include <_PDCLIB_io.h>
#include <stdio.h>
#include <signal.h>

extern int main(int argc, char ** argv);

void _start(){
    unsigned int from;
    unsigned int length = sizeof(unsigned int)*2;
    unsigned int buffer[2];
    unsigned int status = squire_syscall_simple_recv(buffer, &length, &from);
    unsigned int argc = buffer[0];

    unsigned int * buffer2 = (unsigned int*)malloc(buffer[1]);
    char ** argv = (char**)malloc(argc);
    length = buffer[1];
    status = squire_syscall_simple_recv(buffer2, &length, &from);
    unsigned int p = 0;
    for(int i=0; i<argc; i++){
        unsigned int len = buffer2[i];
        char * d = (char*)((unsigned int)buffer2 + sizeof(unsigned int)*argc + p);
        p += len;
        argv[i] = d;
    }

	// Initialize libc stdio file locks (stdin, stdout, stderr)
	stdin->lock = squire_syscall_mutex_init();
	stdout->lock = squire_syscall_mutex_init();
	stderr->lock = squire_syscall_mutex_init();

    // Initialize signal handlers
    signal(0, SIG_DFL);

    int retval = main(argc, argv);

    free(buffer2);
    free(argv);
	squire_syscall_mutex_init(stdin->lock);
	squire_syscall_mutex_init(stdout->lock);
	squire_syscall_mutex_init(stderr->lock);

    squire_syscall_exit(retval);

    for(;;);
}
