#ifndef __H_SQUIRE_SYSCALL
#define __H_SQUIRE_SYSCALL 1

#include <squire_syscall_debug.h>
#include <squire_syscall_memory.h>
#include <squire_syscall_message.h>
#include <squire_syscall_procthread.h>
#include <squire_syscall_mutex.h>
#include <squire_syscall_misc.h>
#include <squire_syscall_io.h>

/**
 * @brief The different syscall groups
 * 
 */
typedef enum SQUIRE_SYSCALL{
    SQUIRE_SYSCALL_MEMORY,      /** @brief Memory operations */
    SQUIRE_SYSCALL_PROCTHREAD,  /** @brief Multithreading operations */
    SQUIRE_SYSCALL_MESSAGE,     /** @brief Messaging and IPC */
    SQUIRE_SYSCALL_IO,          /** @brief Input Output oprations */
    SQUIRE_SYSCALL_DEBUG,       /** @brief Debug operations */
    SQUIRE_SYSCALL_MUTEX,       /** @brief Mutex operations */
    SQUIRE_SYSCALL_MISC         /** @brief Miscelaneous calls */
} squire_syscall_t;

/**
 * @brief The squire system call
 * 
 */
#ifdef __i386__
    #define squire_syscall(GROUP, PARAMSIZE, PARAM) __asm__ __volatile__("int $0x80"::"a"(GROUP),"c"(PARAMSIZE),"d"(PARAM))
#else
    #define squire_syscall(GROUP, PARAMSIZE, PARAM)
#endif

#endif
