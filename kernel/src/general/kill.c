#include <general/kill.h>
#include <general/schedule.h>
#include <general/arch/proc.h>

void kill_childs(proc_proc_t * p){
    proc_proc_t * c = p->childs;
    while(c){
        kill_childs(c);
        c = c->child_next;
    }

    p->killreason = PROC_KILL_REASON_KILLED;

    // Kill all threads of process
    proc_thread_t * t = p->threads;
    while(t){
        schedule_kill(schedule_get(p->id, t->id), 0);
        t = t->next;
    }
}

void kill(unsigned int pid){
    schedule_disable();

    // Get process structure
    proc_proc_t * p = proc_get(pid);
    if(!p){
        printf("Could not find process with PID %d\r\n", pid);
        schedule_enable();
        return;
    }

    kill_childs(p);

    schedule_enable();
}