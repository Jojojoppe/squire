#include <general/schedule.h>

unsigned int schedule_disabled = 0;

void schedule(){
    if(schedule_disabled){
        return;
    }

    proc_proc_t * proc = proc_proc_get_current();
    proc_thread_t * thread = proc_thread_get_current();
    proc_thread_t * next = thread->next;

    // printf("\r\nSWITCH: %08x -> %08x\r\n", thread, thread->next);

    while(1){
        if(next){

            // Check for thread state
            if(next->state == PROC_TRHEAD_STATE_RUNNING){
                proc_thread_switch(next, thread);
                return;
            }

            // Check next thread
            next = next->next;
            continue;
        }

        // Nope, check next process
        proc_proc_switch(proc->next, proc);
        proc = proc->next;
        next = proc->threads;
    }
}

void schedule_disable(){
    schedule_disabled++;
}

void schedule_enable(){
    schedule_disabled--;
}