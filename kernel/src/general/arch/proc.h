#ifndef __H_ARCH_PROC
#define __H_ARCH_PROC 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent process manager
 */

#include <general/config.h>
#include <general/stdint.h>
#include <general/vmm.h>

typedef struct proc_thread_s{
    struct proc_thread_s * next;
    struct proc_thread_s * prev;
    unsigned int id;
    unsigned char arch_data[PROC_THREADDATA_SIZE];
} proc_thread_t;

typedef struct proc_proc_s{
    struct proc_proc_s * next;
    struct proc_proc_s * prev;
    unsigned int id;
    vmm_region_t * memory;
    proc_thread_t * threads;
    unsigned char arch_data[PROC_PROCDATA_SIZE];
} proc_proc_t;

/**
 * @brief Current process
 * 
 */
static proc_proc_t * proc_proc_current;
/**
 * @brief Current thread
 * 
 */
static proc_thread_t * proc_thread_current;
/**
 * @brief The process ID counter. Must be set to 3 at startup.
 * 
 * The value is is used for the next process
 * 
 */
static unsigned int proc_PID_counter;

/**
 * @brief Initialize processes
 * 
 * Sets the current execution as first process/thread and sets it as currently
 * running
 * 
 * @return zero if successful
 */
int proc_init();

/**
 * @brief Switch to thread
 * 
 * @param to  Thread to switch to
 * @param from Thread to switch from. Saves the current state in this structure. 0 if not saving
 * @return zero if successfull
 */
int proc_thread_switch(proc_thread_t * to, proc_proc_t * from);

#endif