#ifndef __H_SCHEDULE
#define __H_SCHEDULE 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent process manager
 */

#include <general/config.h>
#include <general/stdint.h>
#include <general/arch/proc.h>

enum SCHEDULE_QUEUE_TYPE{
    SCHEDULE_QUEUE_TYPE_NORMAL = 0,
    SCHEDULE_QUEUE_TYPE_PRIORITY,
    _SCHEDULE_QUEUE_TYPE_SIZE_
};
typedef unsigned int schedule_queue_type_t;

enum SCHEDULE_STATE{
    SCHEDULE_STATE_RUNNING = 0,
    SCHEDULE_STATE_IDLE
};
typedef unsigned int schedule_state_t;

struct schedule_schedulable_s{
    struct schedule_schedulable_s * next;
    proc_proc_t * process;
    proc_thread_t * thread;
    schedule_state_t state;
};
typedef struct schedule_schedulable_s schedule_schedulable_t;

/**
 * @brief 
 * 
 */
void schedule();

/**
 * @brief 
 * 
 * @param process 
 * @param thread 
 */
void schedule_init(proc_proc_t * process, proc_thread_t * thread);

/**
 * @brief 
 * 
 * @param process 
 * @param thread 
 * @param queue 
 * @return schedule_schedulable_t* 
 */
schedule_schedulable_t * schedule_add(proc_proc_t * process, proc_thread_t * thread, schedule_queue_type_t queue);

/**
 * @brief 
 * 
 * @param schedulable 
 * @param retval
 */
void schedule_kill(schedule_schedulable_t * schedulable, unsigned int retval);

/**
 * @brief 
 * 
 * @param schedulable 
 * @param state 
 */
void schedule_set_state(schedule_schedulable_t * schedulable, schedule_state_t state);

/**
 * @brief 
 * 
 * @param pid 
 * @param tid 
 * @return schedule_schedulable_t* 
 */
schedule_schedulable_t * schedule_get(unsigned int pid, unsigned int tid);

/**
 * @brief Move a task to another queue
 * 
 * @param schedulable 
 * @param queue 
 */
void schedule_move_queue(schedule_schedulable_t * schedulable, schedule_queue_type_t queue);

void schedule_disable();
void schedule_enable();
void schedule_enable_completely();


schedule_queue_type_t schedule_get_current_queue_type();

#endif

