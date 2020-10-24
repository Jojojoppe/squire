#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <squire.h>

int message_queue(void * p){
	printf("Starting message queue\r\n");

	char * buffer = malloc(1024);
	memset(buffer, 0, 1024);
	size_t length = 1024;
	unsigned int from;

	while(!squire_syscall_simple_recv_blocked(buffer, &length, &from)){
		printf("from %d: length: %d %s\r\n", from, length, buffer);
		length = 1024;
	}

	return 0;
}

int main(int argc, char ** argv){
	printf("This is testbin!!!\r\n");
	printf("argc = %08x argv = %08x\r\n", argc, argv);
	for(int i=0; i<argc; i++)
		printf("argv[%d] = %s\r\n", i, argv[i]);

	void * stack = malloc(4096);
	squire_syscall_thread(message_queue, stack, 4096, 0, 0);

	for(;;){
	}

	return 0;
}
