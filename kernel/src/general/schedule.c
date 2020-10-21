#include <general/schedule.h>

unsigned int schedule_disabled = 0;

void schedule(){
    if(schedule_disabled){
        return;
    }

    proc_proc_t * proc = proc_proc_get_current();
    proc_thread_t * thread = proc_thread_get_current();

    // printf("\r\nSWITCH: %08x -> %08x\r\n", thread, thread->next);

    if(thread->next){
        proc_thread_switch(thread->next, thread);
        return;
    }

    // printf("PROC SWITCH: %08x -> %08x\r\n", proc, proc->next);
    proc_proc_switch(proc->next, proc);
}

void schedule_disable(){
    schedule_disabled++;
}

void schedule_enable(){
    schedule_disabled--;
}