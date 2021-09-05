#include "config.h"
#include "kprintf.h"

#include "sysclock.h"

void main(){
	// Start system clock. The system clock is used to count jiffies and for scheduling
	sysclock_init();
	sysclock_enable();

	for(;;){
		kprintf("\r%8d", (unsigned int)sysclock_jiffies());
	}
}
