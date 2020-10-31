#include <general/wait.h>
#include <general/schedule.h>

proc_kill_reason_t wait(unsigned int * retval, unsigned int pid){
    proc_proc_t * pcur = proc_proc_get_current();
    proc_thread_t * tcur = proc_thread_get_current();

    // Get child list
    proc_proc_t * childs = pcur->childs;

    if(pid==0){
        printf("ERROR: wait on all childs not yet implemented\r\n");
        return 0;
    }

    // Find correct child
    proc_proc_t * c = childs;
    while(c){
        if(c->id == pid){
            // Child found
            while(c->threads_number){
                // Wait for child to exit

                c->parentwaitingthread = tcur->id;

                // printf("Wait for child as %d\r\n", tcur->id);
                schedule_set_state(0, SCHEDULE_STATE_IDLE);
                schedule();
                // printf("Woken up!\r\n");
            }
            // TODO free child process structure
            *retval = c->retvalue;
            return c->killreason;
        }
        c = c->child_next;
    }
    // Child not found!
    printf("ERROR: child not found to wait\r\n");
    return 0;
}