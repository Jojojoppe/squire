#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

void SIGTIMhandler(int sig){
	squire_misc_timer_add(10000, squire_misc_timer_get_id()+1);
	printf("%d] Elapsed time: %ld\r\n", squire_misc_timer_get_id(), squire_misc_timer_get_elapsed());
}

int main(int argc, char ** argv){
	signal(SIGTIM, SIGTIMhandler);
	printf("Main thread of init.bin\r\n");

	squire_misc_timer_add(0, 0);

	for(;;){
		printf(".\r\n");
		for(int i=0; i<200000000; i++);
	}

	return 0;
} 
