#ifndef __H_SQUIRE_SYSCALL_DEBUG
#define __H_SQUIRE_SYSCALL_DEBUG 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

/**
 * @brief Debug operations
 * 
 */
typedef enum SQUIRE_SYSCALL_DEBUG_OPERATION{
    /**
     * @brief Kernel log operation
     * 
     * data0:       pointer to message to log
     * length0:     length of the message to log
     */
    SQUIRE_SYSCALL_DEBUG_OPERATION_LOG
} squire_syscall_debug_operation_t;

/**
 * @brief Parameter structure for debug operation
 * 
 */
typedef struct{
    squire_syscall_debug_operation_t        operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    const char *                            data0;          /** @brief First data field */
    size_t                                  length0;        /** @brief First length field */
} squire_syscall_debug_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Log a message in the kernel
 * 
 * @param message The message to be logged
 * @param length The length of the message
 * @return size_t The length of the logged message
 */
extern size_t squire_debug_log(const char * message, size_t length);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif