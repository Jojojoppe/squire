#include <general/timer.h>

#include <general/kprintf.h>
#include <general/config.h>
#include <general/schedule.h>
#include <general/kill.h>
#include <general/kmalloc.h>

unsigned long long timer_value;
timer_callback_t * timer_callbacks;

void timer_interrupt(){
    timer_value++;

    // Check timer callbacks
    if(timer_callbacks){
        timer_callback_t * t = timer_callbacks;
        while(t){
            // Check if may send signal
            if(timer_value>=t->timer_value){
                // Send signal
                // TODO use SIGTIM
                unsigned long long elapsed = timer_value - t->start_value;
                unsigned int low = (unsigned int)elapsed;
                unsigned int high = (unsigned int)(elapsed>>32);
                kill_extra(t->PID, KILL_REASON_TIM, low, high, t->id, 0);
                timer_callback_t * tmp = t;
                timer_callbacks = t->next;
                t = t->next;
                kfree(tmp);
            }else{
                // Not yet time to send signal
                break;
            }
        }
    }

    if((timer_value&0x03)==0x03)
        schedule();
}

unsigned int timer_get(){
    return (unsigned int) timer_value;
}

void timer_add_callback(unsigned int PID, unsigned long long time, unsigned int id){
    unsigned long long tendval = timer_value + time;
    timer_callback_t * t = (timer_callback_t*)kmalloc(sizeof(timer_callback_t));
    t->next = 0;
    t->PID = PID;
    t->timer_value = tendval;
    t->start_value = timer_value;
    t->id = id;
    timer_callback_t * callbacks = timer_callbacks;
    if(callbacks){
        // Add to list
        timer_callback_t * prev = callbacks;
        while(callbacks){
            if(prev->timer_value<tendval){
                if(callbacks->next && callbacks->next->timer_value>=tendval){
                    // Place between prev and callbacks
                    t->next = prev->next;
                    prev->next = t;
                    return;
                }
            }
            prev = callbacks;
            callbacks = callbacks->next;
        }
        // Must be placed after prev
        prev->next = t;
        return;
    }else{
        timer_callbacks = t;
    }
}