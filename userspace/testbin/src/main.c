#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <squire.h>

void sighanlder(int signal){
	printf("SIGNAL: %d\r\n", signal);

	if(signal==20){
		exit(-1);
	}

	return 0;
}

int main(int argc, char ** argv){
	printf("This is testbin.bin\r\n");

	squire_syscall_signal(sighanlder);

	for(int i=0; i<200000000; i++);
	squire_syscall_kill(0, 20);

	for(int i=0; i<200000000; i++);

	return 0;
} 
