#ifndef __H_SQUIRE_SYSCALL_MESSAGE
#define __H_SQUIRE_SYSCALL_MESSAGE 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

// Operation flags
#define RECEIVE_BLOCKED     1       /** @brief Receiving is blocked */

/**
 * @brief Message operation return statii
 * 
 */
typedef enum SQUIRE_MESSAGE_STATUS{
    SQUIRE_MESSAGE_STATUS_NOERROR = 0,      /** @brief No error */
    SQUIRE_MESSAGE_STATUS_MSGTOSHORT,       /** @brief Message is too short, must be minimum 8 byte */
    SQUIRE_MESSAGE_STATUS_RECUNKNOWN,       /** @brief Receiver unknown: invalid PID */
    SQUIRE_MESSAGE_STATUS_BUFFTOSHORT,      /** @brief Buffer is to short to fill with the complete message */
    SQUIRE_MESSAGE_STATUS_NOMSG,            /** @brief No message in the queue */
    SQUIRE_MESSAGE_STATUS_ALRBLOCK          /** @brief Already a blocking receive executing on this queue */
} squire_message_status_t;

/**
 * @brief Message operations
 * 
 */
typedef enum SQUIRE_SYSCALL_MESSAGE_OPERATION{
    /**
     * @brief Send a simple message
     * 
     * Sends a message to a processes simple queue.
     * data0:       Pointer to the data to be sent
     * length0:     Length of the data to be sent
     * to0:         PID of the receiving process
     * After execution return0 contains the status of the execution following
     * squire_message_status_t
     */
    SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_SEND,
    /**
     * @brief Receive a simple message
     * 
     * Receive a message from its own simple queue. A blocked receiving is executed
     * by adding the RECEIVE_BLOCKED flag. If not the operation will be returned directly
     * data0:       Pointer to a bufer
     * length0:     Length of the buffer
     * After execution return0 contains the status of the execution following
     * squire_message_status_t. length0 contains the length of the received message
     * in case of successful reception, in case of error it contains the length of the
     * next message. from0 contains the PID of the sending process
     */
    SQUIRE_SYSCALL_MESSAGE_OPERATION_SIMPLE_RECEIVE
} squire_syscall_message_operation_t;

/**
 * @brief Parameter structure for message operation
 * 
 */
typedef struct{
    squire_syscall_message_operation_t      operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
    void *                                  data0;          /** @brief first data */
    size_t                                  length0;        /** @brief first length */
    unsigned int                            from0;          /** @brief first from */
    unsigned int                            to0;            /** @brief first to */
    int                                     return0;        /** @brief first return value */
} squire_syscall_message_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Send a message to the simple queue
 * 
 * @param buffer Pointer to a region containing the message
 * @param length The length of the message
 * @param to The PID of the receiving process
 * @return squire_message_status_t Return status of the function
 */
extern squire_message_status_t squire_message_simple_send(void * buffer, size_t length, unsigned int to);

/**
 * @brief Receive a message from the simple queue
 * 
 * @param buffer Pointer to buffer
 * @param length Pointer to length of buffer. Contains length of message after execution
 * @param from Pointer to variable storing from PID
 * @param flags Flags
 * @return squire_message_status_t Return status of the function
 */
extern squire_message_status_t squire_message_simple_receive(void * buffer, size_t * length, unsigned int * from, int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif