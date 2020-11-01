#include <i386/timer.h>

int timer_init(){

    // Initialize timer at 100 hz
    unsigned short timer_val = 1193180 / 100;
    io_outb(0x43, 0x36);
    io_outb(0x40, timer_val);
    io_outb(0x40, timer_val >> 8);


    extern timer_callback_t * timer_callbacks;
    timer_callbacks = 0;
    extern unsigned long long timer_value;
    timer_value = 0;
    
    __asm__ __volatile__("sti");    

    return 0;
}
