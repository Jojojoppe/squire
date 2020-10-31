#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <squire.h>

void sighanlder(int signal){
	printf("SIGNAL: %d\r\n", signal);

	if(signal==20){
		exit(5);
	}

}

int main(int argc, char ** argv){
	printf("This is testbin.bin\r\n");

	for(int i=0; i<200000000; i++);
	raise(SIGTERM);

	for(int i=0; i<200000000; i++);

	return 0;
} 
