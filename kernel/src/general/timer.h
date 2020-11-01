#ifndef __H_TIMER
#define __H_TIMER 1

typedef struct timer_callback_s{
    unsigned long long timer_value;
    unsigned long long start_value;
    unsigned int PID;
    unsigned int id;
    struct timer_callback_s * next;
} timer_callback_t;


void timer_add_callback(unsigned int PID, unsigned long long time, unsigned int id);

void timer_interrupt();
void timer_update();
unsigned int timer_get();

#endif