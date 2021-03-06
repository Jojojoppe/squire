#include <general/schedule.h>
#include <general/arch/arch.h>
#include <general/kmalloc.h>

unsigned int schedule_disabled = 0;

schedule_schedulable_t * schedule_current;
schedule_queue_type_t schedule_current_queue_type = 0;
// Schedule queues
schedule_schedulable_t * schedule_queues[_SCHEDULE_QUEUE_TYPE_SIZE_];

void schedule_debug(){
    for(int i=0; i<_SCHEDULE_QUEUE_TYPE_SIZE_; i++){
        schedule_schedulable_t * s = schedule_queues[i];
        printf("QUEUE %d\r\n", i);
        while(s){
            printf("%d-%d [%d]\r\n", s->process->id, s->thread->id, s->state);
            s = s->next;
        }
    }
}

void schedule_init(proc_proc_t * process, proc_thread_t * thread){
    // Clear out queues
    for(int i=0; i<_SCHEDULE_QUEUE_TYPE_SIZE_; i++){
        schedule_queues[i] = 0;
    }

    schedule_current = schedule_add(process, thread, SCHEDULE_QUEUE_TYPE_NORMAL);
    schedule_current_queue_type = SCHEDULE_QUEUE_TYPE_NORMAL;
}

schedule_schedulable_t * schedule_add(proc_proc_t * process, proc_thread_t * thread, schedule_queue_type_t queue){
    //printf("** schedule_add(%08x, %08x, %d)\r\n", process, thread, queue);
    schedule_schedulable_t * schedule_new = (schedule_schedulable_t*)kmalloc(sizeof(schedule_schedulable_t));
    schedule_new->next = 0;
    schedule_new->process = process;
    schedule_new->thread = thread;
    schedule_new->state = SCHEDULE_STATE_RUNNING;

    // Add to end of queue
    schedule_schedulable_t * entry = schedule_queues[queue];
    if(!entry){
        // First entry in queue
        schedule_queues[queue] = schedule_new;
    }else{
        // Link to end of list
        while(entry->next){
            entry = entry->next;
        }
        // entry is last in queue
        entry->next = schedule_new;
    }

    return schedule_new;
}

void schedule_kill(schedule_schedulable_t * schedulable, unsigned int retval){
    // schedule_disable();
    //printf("** schedule_kill(%08x, %08x)\r\n", schedulable, retval);
    if(!schedulable){
        schedulable = schedule_current;
        schedule_current = 0;
    }

    for(int i=0; i<_SCHEDULE_QUEUE_TYPE_SIZE_; i++){
        schedule_schedulable_t * prev = schedule_queues[i];
        if(prev==schedulable){
            kfree(schedulable);
            proc_thread_kill(schedulable->thread, schedulable->process, retval);
            return;
        }
        while(prev){
            if(prev->next==schedulable){
                prev->next = prev->next->next;
                kfree(schedulable);
                proc_thread_kill(schedulable->thread, schedulable->process, retval);
                return;
            }
            prev = prev->next;
        }
    }
}

void schedule_set_state(schedule_schedulable_t * schedulable, schedule_state_t state){
    //printf("** schedule_set_state(%08x, %d)\r\n", schedulable, state);
    if(!schedulable){
        schedulable = schedule_current;
    }
    schedulable->state = state;
}

void schedule(){
    if(schedule_disabled){
        return;
    }

    schedule_schedulable_t * current = schedule_current;
    schedule_schedulable_t * try = current;
    schedule_schedulable_t * next = 0;

    if(!schedule_current){
        // printf("schedule_current does not exist\r\n");
        try = schedule_queues[SCHEDULE_QUEUE_TYPE_NORMAL];
    }

    while(!next){
        // Check if next in priority queue
		schedule_current_queue_type = SCHEDULE_QUEUE_TYPE_PRIORITY;
        if(schedule_queues[SCHEDULE_QUEUE_TYPE_PRIORITY]){
            // Switch to one in priority
            schedule_schedulable_t * s = schedule_queues[SCHEDULE_QUEUE_TYPE_PRIORITY];
            while(s){
                if(s->state == SCHEDULE_STATE_RUNNING){
                    next = s;
                    break;
                }
                s = s->next;
            }
			if(next)
				break;
        }

        // Otherwise check for next in normal queue
		schedule_current_queue_type = SCHEDULE_QUEUE_TYPE_NORMAL;
		if(try->next){
			if(try->next->state != SCHEDULE_STATE_RUNNING){
				try = try->next;
			}else{
				// Schedule to next
				next = try->next;
			}
		}else{
			// Schedule start of queue
			try = schedule_queues[SCHEDULE_QUEUE_TYPE_NORMAL];
			if(try->state == SCHEDULE_STATE_RUNNING){
				next = try;
			}else{
				// End of queue
			}
		}
    }

    // Do nothing if there's no new next one
    if(current==next)
        return;

    schedule_current = next;
    if(!current){
        // printf("** schedule() P(%d->%d) T(%d->%d)\r\n", 0, next->process->id, 0, next->thread->id);
        proc_switch(next->thread, 0, next->process, 0);
    }else{
        // printf("** schedule() P(%d->%d) T(%d->%d)\r\n", current->process->id, next->process->id, current->thread->id, next->thread->id);
        proc_switch(next->thread, current->thread, next->process, current->process);
    }
}

schedule_schedulable_t * schedule_get(unsigned int pid, unsigned int tid){
	if(pid==0 && tid==0){
		return schedule_current;
	}
    for(int i=0; i<_SCHEDULE_QUEUE_TYPE_SIZE_; i++){
        schedule_schedulable_t * s = schedule_queues[i];
        while(s){
            // Check for pid and tid
            if(s->process->id==pid && s->thread->id==tid){
                return s;
            }

            s = s->next;
        }
    }
    return 0;
}

void schedule_disable(){
    schedule_disabled++;
    //arch_disable_interrupts();
    // printf("** schedule_disable() %d\r\n", schedule_disabled);
}

void schedule_enable(){
    schedule_disabled--;
    //if(!schedule_disabled)
    //    arch_enable_interrupts();
    // printf("** schedule_enable() %d\r\n", schedule_disabled);
}

void schedule_enable_completely(){
    schedule_disabled = 0;
}

void schedule_move_queue(schedule_schedulable_t * schedulable, schedule_queue_type_t queue){
    schedule_disable();
    // printf("schedule_move_queue(%08x, %d)\r\n", schedulable, queue);

    // Find in queue
    for(int q=0; q<_SCHEDULE_QUEUE_TYPE_SIZE_; q++){
        schedule_schedulable_t * prev = 0;
        schedule_schedulable_t * s = schedule_queues[q];
        while(s){

            if(s==schedulable){
                // Found!
                // Unlink from current queue
                if(prev){
                    // middle of queue
                    prev->next = s->next;
                }else{
                    // Start of queue
                    schedule_queues[q] = s->next;
                }
                // Add to new queue
                s = schedule_queues[queue];
                if(s){
                    // Add to end
                    while(s->next)
                        s = s->next;
                    s->next = schedulable;
                }else{
                    // First in queue
                    schedule_queues[queue] = schedulable;
                }

                schedule_enable();
                return;
            }

            prev = s;
            s = s->next;
        }
    }
    // Not found. Do nothing
    schedule_enable();
}

schedule_queue_type_t schedule_get_current_queue_type(){
	return schedule_current_queue_type;
}
