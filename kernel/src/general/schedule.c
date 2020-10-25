#include <general/schedule.h>

unsigned int schedule_disabled = 0;

void schedule(){
    if(schedule_disabled){
        return;
    }

    proc_proc_t * proc = proc_proc_get_current();
    proc_thread_t * thread = proc_thread_get_current();
    proc_thread_t * next = thread->next;

    printf("SWITCH: ");

    while(1){
        if(next){

            // Check for thread state
            if(next->state == PROC_TRHEAD_STATE_RUNNING){
                printf("T[%08x %d]->[%08x %d]\r\n", thread, thread->id, next, next->id);
                proc_thread_switch(next, thread);
                return;
            }

            // Check next thread
            next = next->next;
            continue;
        }

        // Nope, check next process
        printf("P[%08x %d]->[%08x %d] t@%08x ", proc, proc->id, proc->next, proc->next->id, proc->next->threads);
        proc_proc_switch(proc->next, proc);
        proc = proc->next;
        next = proc->threads;
    }
}

void schedule_disable(){
    schedule_disabled++;
    // printf("> schedule_disabled=%d\r\n", schedule_disabled);
    __asm__ __volatile__("cli");
}

void schedule_enable(){
    schedule_disabled--;
    // printf("< schedule_disabled=%d\r\n", schedule_disabled);
    if(!schedule_disabled)
        __asm__ __volatile__("sti");
}