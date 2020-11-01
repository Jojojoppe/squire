#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

void SIGTIMhandler(int sig){
	squire_misc_timer_add(SQUIRE_HZ*0.5, squire_misc_timer_get_id()+1);
	printf("%d] Elapsed time: %ld\r\n", squire_misc_timer_get_id(), squire_misc_timer_get_elapsed());
}

int main(int argc, char ** argv){
	signal(SIGTIM, SIGTIMhandler);
	printf("Main thread of init.bin\r\n");

	squire_misc_timer_add(0, 0);

	char v = 0;
	asm volatile("in %%dx, %%ax":"=a"(v):"d"(0x20));
	printf("v=%02x\r\n", v);

	for(;;){
	}

	return 0;
} 
