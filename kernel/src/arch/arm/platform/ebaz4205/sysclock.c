#include "../../sysclock.h"
#include "../../../../common/kprintf.h"
#include "../../gic.h"
#include "../../cpu.h"

#define PRIVATE_TIMER_BASE (0x0600 + cpu_get_periphbase())

#define POINTER_TO_REGISTER(REG)  ( *((volatile unsigned int*)(REG)))
#define PRIVATE_TIMER_LOAD POINTER_TO_REGISTER(PRIVATE_TIMER_BASE)
#define PRIVATE_TIMER_CURRENT POINTER_TO_REGISTER(PRIVATE_TIMER_BASE+4)
#define PRIVATE_TIMER_CONTROL POINTER_TO_REGISTER(PRIVATE_TIMER_BASE+8)
#define PRIVATE_TIMER_F POINTER_TO_REGISTER(PRIVATE_TIMER_BASE+12)

#define PRIVATE_TIMER_ENABLE 1
#define PRIVATE_TIMER_AUTO 2
#define PRIVATE_TIMER_INTERRUPT 4

void arch_sysclock_init(){
    kprintf("arch_sysclock_init\r\n");

    PRIVATE_TIMER_LOAD = 1000000;
    PRIVATE_TIMER_CONTROL = PRIVATE_TIMER_ENABLE | PRIVATE_TIMER_INTERRUPT | PRIVATE_TIMER_AUTO;
}

void arch_sysclock_enable(){
    gic_enable_interrupt(29, 0);
}

void arch_sysclock_disable(){
    //gic_disable_interrupt(29);
}

void arch_sysclock_isr(){
    PRIVATE_TIMER_F = 0;
    sysclock_cb();
}