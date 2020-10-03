#include <general/kprintf.h>
#include <general/arch/timer.h>
#include <general/config.h>

static unsigned long timer_value;

void timer_interrupt(){
    timer_value++;
}

unsigned int timer_get(){
    return (unsigned int) timer_value;
}