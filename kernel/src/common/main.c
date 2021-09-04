#include "config.h"
#include "kprintf.h"

#include "sysclock.h"

void main(){
	// Start system clock. The system clock is used to count jiffies and for scheduling
	sysclock_init();
	sysclock_enable();
}
