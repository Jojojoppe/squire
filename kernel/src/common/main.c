#include "config.h"
#include "kprintf.h"

#include "sysclock.h"

void main(){

	sysclock_init();
	sysclock_enable();

	for(;;);
}
