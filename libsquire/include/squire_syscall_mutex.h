#ifndef __H_SQUIRE_SYSCALL_MUTEX
#define __H_SQUIRE_SYSCALL_MUTEX 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

/**
 * @brief The mutex type
 * 
 */
typedef void * squire_mutex_t;

/**
 * @brief Mutex operations
 * 
 */
typedef enum SQUIRE_SYSCALL_MUTEX_OPERATION{
    /**
     * @brief Create a mutex
     * 
     * After execution mtx0 contains the mutex variable
     */
    SQUIRE_SYSCALL_MUTEX_OPERATION_CREATE,
    /**
     * @brief Destroy a mutex
     * 
     * mtx0:    The mutex to destroy
     */
    SQUIRE_SYSCALL_MUTEX_OPERATION_DESTROY,
    /**
     * @brief Lock a mutex
     * 
     * mtx0:    The mutex
     */
    SQUIRE_SYSCALL_MUTEX_OPERATION_LOCK,
    /**
     * @brief Unlock a mutex
     * 
     * mtx0:    The mutex
     */
    SQUIRE_SYSCALL_MUTEX_OPERATION_UNLOCK,
    /**
     * @brief Get the status of a mutex
     * 
     * mtx0:    The mutex
     * After execution value0 contains the status. -1 if mutex does not exists
     */
    SQUIRE_SYSCALL_MUTEX_OPERATION_STATUS
} squire_syscall_mutex_operation_t;

/**
 * @brief Parameter structure for mutex operation
 * 
 */
typedef struct{
    squire_syscall_mutex_operation_t        operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    squire_mutex_t                          mtx0;
    int                                     value0;
} squire_syscall_mutex_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Create a mutex
 * 
 * @return squire_mutex_t The newly created mutex
 */
extern squire_mutex_t squire_mutex_create();

/**
 * @brief Destroy a mutex
 * 
 * @param mtx 
 */
extern void squire_mutex_destroy(squire_mutex_t mtx);

/**
 * @brief Lock a mutex
 * 
 * @param mtx 
 */
extern void squire_mutex_lock(squire_mutex_t mtx);

/**
 * @brief Unlock a mutex
 * 
 * @param mtx 
 */
extern void squire_mutex_unlock(squire_mutex_t mtx);

/**
 * @brief Get status of mutex
 * 
 * @param mtx 
 * @return int Status of mutex, -1 if non existent
 */
extern int squire_mutex_status(squire_mutex_t mtx);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif