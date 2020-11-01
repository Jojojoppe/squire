#include <_PDCLIB_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <squire.h>

extern int main(int argc, char ** argv);

void _start(){
    unsigned int from;
    unsigned int length = sizeof(unsigned int)*2;
    unsigned int buffer[2];
    squire_message_simple_receive(buffer, &length, &from, RECEIVE_BLOCKED);
    unsigned int argc = buffer[0];

    unsigned int * buffer2 = (unsigned int*)malloc(buffer[1]);
    char ** argv = (char**)malloc(argc);
    length = buffer[1];
    squire_message_simple_receive(buffer2, &length, &from, RECEIVE_BLOCKED);
    unsigned int p = 0;
    for(int i=0; i<argc; i++){
        unsigned int len = buffer2[i];
        char * d = (char*)((unsigned int)buffer2 + sizeof(unsigned int)*argc + p);
        p += len;
        argv[i] = d;
    }

	// Initialize libc stdio file locks (stdin, stdout, stderr)
	stdin->lock = squire_mutex_create();
	stdout->lock = squire_mutex_create();
	stderr->lock = squire_mutex_create();

    // Initialize signal handlers
    signal(0, SIG_DFL);

    int retval = main(argc, argv);

    free(buffer2);
    free(argv);
    squire_mutex_destroy(stdin->lock);
    squire_mutex_destroy(stdout->lock);
    squire_mutex_destroy(stderr->lock);

    squire_procthread_exit_process(retval);

    for(;;);
}
