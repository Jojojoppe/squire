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
    void * stack;
    size_t stack_length;
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
 * @param return_addr Address to jump to when done with initialization
 * @return zero if successful
 */
int proc_init(void (*return_addr)());

/**
 * @brief Switch to thread
 * 
 * @param to  Thread to switch to
 * @param from Thread to switch from. Saves the current state in this structure. 0 if not saving
 * @return zero if successfull
 */
int proc_thread_switch(proc_thread_t * to, proc_thread_t * from);

/**
 * @brief Switch to process
 * 
 * @param to Process to switch to
 * @param from Process to switch from. Saves the current state in this structure
 * @return zero if successfull
 */
int proc_proc_switch(proc_proc_t * to, proc_proc_t * from);

/**
 * @brief Execute in user thread
 * 
 * Sets up a user stack and kernel stack and jumps to ring 3. Paramters are stored
 * in archtitecture specific way. This function is used as return address for the
 * start of a user thread
 */
void proc_user_exec();

/**
 * @brief Get current thread
 * 
 * @return current thread
 */
proc_thread_t * proc_thread_get_current();

/**
 * @brief Get current process
 * 
 * @return current process
 */
proc_proc_t * proc_proc_get_current();

/**
 * @brief Create new thread
 * 
 * @param code Code to execute
 * @param stack Stack base
 * @param stack_length Length of stack
 * @param process Process to add thread to
 * @return Created thread structure, NULL if not successfull
 */
proc_thread_t * proc_thread_new(void * code, void * stack, size_t stack_length, proc_proc_t * process);

/**
 * @brief Get memory list of process
 * 
 * @return vmm_region_t* memory list
 */
vmm_region_t * proc_get_memory();

/**
 * @brief Set memory list
 * 
 * @param region 
 */
void proc_set_memory(vmm_region_t * region);

#endif