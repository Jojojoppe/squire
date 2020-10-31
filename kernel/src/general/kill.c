#include <general/kill.h>
#include <general/schedule.h>
#include <general/arch/proc.h>

void kill_childs(proc_proc_t * p, kill_reason_t reason){
    // TODO what to do with the children?
    proc_proc_t * c = p->childs;
    while(c){
        kill_childs(c, KILL_REASON_TERM);
        c = c->child_next;
    }

    p->killreason = reason;

    // Kill all threads of process
    proc_thread_t * t = p->threads;
    proc_thread_t * current = 0;
    while(t){
        if(t==proc_thread_get_current()){
            current = t;
            t = t->next;
            continue;
        }
        proc_thread_t * next = t->next;
        schedule_kill(schedule_get(p->id, t->id), 0);
        t = p->threads;
    }
    if(current){
        schedule_kill(schedule_get(p->id, current->id), reason);
    }
}

void kill(unsigned int pid, kill_reason_t reason){
    schedule_disable();

    if(pid==0){
        pid = proc_proc_get_current()->id;
    }

    // Get process structure
    proc_proc_t * p = proc_get(pid);
    if(!p){
        printf("Could not find process with PID %d\r\n", pid);
        schedule_enable();
        return;
    }

    // Check if hanlder is installed and may be handled
    if(p->signal_handler && reason != KILL_REASON_KILL){
        // Add a signal to the signal list
        signal_t * signals = p->signals;
        signal_t * newsignal = (signal_t*) kmalloc(sizeof(signal_t));
        newsignal->value = reason;
        newsignal->next;
        newsignal->source_tid = proc_thread_get_current()->id;
        if(signals){
            while(signals->next)
                signals = signals->next;
            signals->next = newsignal;
        }else{
            p->signals = newsignal;
        }

        // Wake up signal handler
        schedule_set_state(schedule_get(p->id, p->signal_handler), SCHEDULE_STATE_RUNNING);
    }else{
        // No signal handler: default behaviour
        switch(reason){
            case KILL_REASON_ABRT:
            case KILL_REASON_TERM:
            case KILL_REASON_INT:
            case KILL_REASON_ETC:
            case KILL_REASON_FPE:
            case KILL_REASON_SEGV:
            case KILL_REASON_ILL:
            case KILL_REASON_KILL:
                kill_childs(p, reason);
                break;

            default:
                break;
        }
    }

    schedule_enable();
    schedule();
}

void exit(unsigned int retval){
    schedule_disable();
    proc_proc_t * pcur = proc_proc_get_current();

    if(pcur->childs)
        kill_childs(pcur->childs, KILL_REASON_TERM);

    pcur->killreason = KILL_REASON_TERM;

    // Kill all threads of process
    proc_thread_t * t = pcur->threads;
    proc_thread_t * current = 0;
    while(t){
        if(t==proc_thread_get_current()){
            current = t;
            t = t->next;
            continue;
        }
        proc_thread_t * next = t->next;
        schedule_kill(schedule_get(pcur->id, t->id), 0);
        t = pcur->threads;
    }
    if(current){
        schedule_kill(schedule_get(pcur->id, current->id), retval);
    }

    schedule_enable();
    schedule();
}
