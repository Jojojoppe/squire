#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <squire.h>

void sighanlder(int signal){
	printf("SIGNAL: %d\r\n", signal);
	printf("Some correct cleanup\r\n");
	exit(0);
}

int main(int argc, char ** argv){
	printf("This is testbin.bin\r\n");

	signal(SIGTERM, sighanlder);

	for(;;);
	return 0;
} 
