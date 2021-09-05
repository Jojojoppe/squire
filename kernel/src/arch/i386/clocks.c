#include "../../common/arch/clocks.h"
#include "../../common/kprintf.h"

void arch_sysclock_init(){
}

void arch_sysclock_enable(){
}

void arch_sysclock_disable(){
}

void arch_sysclock_isr(){
	sysclock_cb();
}
