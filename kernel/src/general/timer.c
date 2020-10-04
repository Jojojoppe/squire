#include <general/kprintf.h>
#include <general/arch/timer.h>
#include <general/config.h>
#include <general/schedule.h>

static unsigned long timer_value;

void timer_interrupt(){
    timer_value++;

    if((timer_value&0x07)==0x07)
        schedule();
}

unsigned int timer_get(){
    return (unsigned int) timer_value;
}