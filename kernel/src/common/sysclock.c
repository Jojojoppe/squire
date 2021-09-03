#include "sysclock.h"
#include "arch/clocks.h"

unsigned long long sysclock_jiffies;

void sysclock_init(){
    sysclock_jiffies = 0;

    arch_sysclock_init();
}

void sysclock_enable(){
    arch_sysclock_enable();
}

void sysclock_disable(){
    arch_sysclock_disable();
}

void sysclock_cb(){
    sysclock_jiffies++;
}
