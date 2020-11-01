#ifndef __H_SQUIRE_SYSCALL_PROCTHREAD
#define __H_SQUIRE_SYSCALL_PROCTHREAD 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

// Operation flags
#define THREAD_QUEUE_PRIORITY   1       /** @brief Place a thread in priority queue */

/**
 * @brief Multithreading operations
 * 
 */
typedef enum SQUIRE_SYSCALL_PROCTHREAD_OPERATION{
    /**
     * @brief Create a thread
     * 
     * Creates a new thread. If THREAD_QUEUE_PRIORITY is set the new thread is placed in
     * priority queue
     * func0:       The entry of the new thread
     * address0:    The base of the stack
     * length0:     The length of the stack
     * address1:    Parameter for the new thread
     * After execution tid0 contains the TID of the new thread
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_THREAD,
    /**
     * @brief Create a process
     * 
     * address0:        Start of ELF file
     * length0:         Length of ELF file
     * value0:          argc
     * address1:        Start of parameter data
     * length1:         Length of parameter data
     * After execution pid0 contains the PID of the new child process 
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_PROCESS,
    /**
     * @brief Get current process and thread id's
     * 
     * After execution tid0 and pid0 contains the TID and PID of the
     * current thread and process
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GET_IDS,
    /**
     * @brief Join with a thread
     * 
     * tid0:        TID of the thread to join with
     * After execution value0 contains the return value
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_JOIN_THREAD,
    /**
     * @brief Exit current thread
     * 
     * value0:      Thread return value
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_EXIT_THREAD,
    /**
     * @brief Exit current process
     * 
     * value0:      Process return value
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_EXIT_PROCESS,
    /**
     * @brief Wait on the exit of a child process
     * 
     * pid0:        PID of child to wait for. 0 if waiting for all childs
     * After execution value0 contains the signal which caused the process to exit
     * and value1 contains the return value
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_WAIT_PROCESS,
    /**
     * @brief Register signal handler
     * 
     * Register an already created thread as signal handler. The thread
     * should execute the blocking SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GETSIG
     * and will be woken up after a signal is received
     * tid0:        TID of the thread
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_SIGNAL,
    /**
     * @brief Wait for a signal
     * 
     * After execution value0 contains the signal
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GETSIG,
    /**
     * @brief Send a signal to a process
     * 
     * pid0:        PID of process to send signal to
     * value0:      signal
     */
    SQUIRE_SYSCALL_PROCTHREAD_OPERATION_KILL,
} squire_syscall_procthread_operation_t;

/**
 * @brief Parameter structure for multithreading operation
 * 
 */
typedef struct{
    squire_syscall_procthread_operation_t   operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    unsigned int                            pid0;
    unsigned int                            tid0;
    void (*func0)();
    void *                                  address0;
    void *                                  address1;
    size_t                                  length0;
    size_t                                  length1;
    int                                     value0;
    int                                     value1;
} squire_syscall_procthread_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Create a thread
 * 
 * @param entry Entry of the thread
 * @param stack_base Base of the stack for the thread
 * @param stack_length Length of the stack for the thread
 * @param flags
 * @param param Parameter for the thread
 * @return unsigned int TID of the new thread
 */
extern unsigned int squire_procthread_create_thread(int (*entry)(void*), void * stack_base, size_t stack_length, int flags, void * param);

/**
 * @brief Create a new process
 * 
 * @param elf_start Start of ELF file
 * @param elf_length Length of ELF file
 * @param argc argc
 * @param argv argv
 * @return unsigned int PID of new process
 */
extern unsigned int squire_procthread_create_process(void * elf_start, size_t elf_length, int argc, char ** argv);

/**
 * @brief Exit the current thread
 * 
 * @param returnvalue The return value of the thread
 */
extern void squire_procthread_exit_thread(int returnvalue);

/**
 * @brief Exit the current process
 * 
 * @param returnvalue The return value of the process
 */
extern void squire_procthread_exit_process(int returnvalue);

/**
 * @brief Get PID of current process
 * 
 * @return unsigned int PID
 */
extern unsigned int squire_procthread_getpid();

/**
 * @brief Get TID of current thread
 * 
 * @return unsigned int TID
 */
extern unsigned int squire_procthread_gettid();

/**
 * @brief Wait for a child process
 * 
 * @param pid PID of the child process. 0 if all childs
 * @param returnvalue Pointer to where the return value is stored. 0 if not stored
 * @return int Terminating signal of the process
 */
extern int squire_procthread_wait(unsigned int pid, int * returnvalue);

/**
 * @brief Join a thread
 * 
 * @param tid TID of the thread
 * @return int Return value of the thread
 */
extern int squire_procthread_join(unsigned int tid);

/**
 * @brief Send signal to process
 * 
 * @param pid PID of process
 * @param signal signal to send
 */
extern void squire_procthread_kill(unsigned int pid, int signal);

/**
 * @brief Registers a function as signal handler
 * 
 * Sets up stack and uses a wrapper function to call SQUIRE_SYSCALL_PROCTHREAD_OPERATION_GETSIG
 * 
 * @param handler Entry of signal handler
 */
extern void squire_procthread_signal(void (*handler)(int));

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif