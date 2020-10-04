#include <general/schedule.h>

void schedule(){
    proc_proc_t * proc = proc_proc_get_current();
    proc_thread_t * thread = proc_thread_get_current();

    printf("SWITCH: %08x -> %08x\r\n", thread, thread->next);

    if(thread->next){
        proc_thread_switch(thread->next, thread);
        return;
    }

    printf("PROC SWITCH: %08x -> %08x\r\n", proc, proc->next);
    proc_proc_switch(proc->next, proc);

}