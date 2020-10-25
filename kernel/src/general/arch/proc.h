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
#include <general/message.h>

typedef enum proc_thread_state_e{
    PROC_TRHEAD_STATE_RUNNING = 0,
    PROC_THREAD_STATE_WAITING
} proc_thread_state_t;

typedef struct proc_thread_s{
    struct proc_thread_s * next;
    struct proc_thread_s * prev;
    unsigned int id;

    void * stack;
    size_t stack_length;
    void * kernel_stack;
    size_t kernel_stack_length;

    int retval;
    proc_thread_state_t state;
    unsigned char arch_data[PROC_THREADDATA_SIZE];
} proc_thread_t;

typedef struct proc_proc_s{
    struct proc_proc_s * next;
    struct proc_proc_s * prev;
    unsigned int id;
    vmm_region_t * memory;

    proc_thread_t * threads;
    proc_thread_t * killed_threads;

    unsigned int kernel_stacks;

    message_info_t message_info;
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
 * @brief 
 * 
 * @param tto 
 * @param tfon 
 * @param pto 
 * @param pfrom 
 */
void proc_switch(proc_thread_t * tto, proc_thread_t * tfon, proc_proc_t * pto, proc_proc_t * pfrom);

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
 * @brief Create new user thread
 * 
 * @param code Code to execute
 * @param stack Stack base
 * @param stack_length Length of stack
 * @param process Process to add thread to
 * @return Created thread structure, NULL if not successfull
 */
proc_thread_t * proc_thread_new_user(void * code, void * stack, size_t stack_length, proc_proc_t * process);

/**
 * @brief Stop a thread
 * 
 * @param thread Thread to stop
 * @param process Process in which thread runs
 * @param retval Return value of thread
 * @return Zero if successful
 */
int proc_thread_kill(proc_thread_t * thread, proc_proc_t * process, int retval);

/**
 * @brief Start a new process
 * 
 * @param ELF_start Start of ELF data
 * @return proc_proc_t* 
 */
proc_proc_t * proc_proc_new(void * ELF_start);

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

/**
 * @brief Print process list with all threads
 * 
 */
void proc_debug();

/**
 * @brief Get the process structure from its id
 * 
 * @param PID The process ID
 * @return The process structure, NULL if not found
 */
proc_proc_t * proc_get(unsigned int PID);

/**
 * @brief Get the thread structure from its id
 * 
 * @param tid 
 * @param pid 
 * @return proc_thread_t* 
 */
proc_thread_t * proc_thread_get(unsigned int tid, unsigned int pid);

/**
 * @brief Fork the current process
 * 
 * @return The new process structure of the child
 */
proc_proc_t * proc_proc_fork();

#endif