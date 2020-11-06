#ifndef __H_MESSAGE
#define __H_MESSAGE 1
/**
 * @file
 * @section DESCRIPTION
 * 
 */

#include <general/config.h>
#include <general/stdint.h>

/**
 * @brief The simple message structure
 * 
 * The structure contains the sender PID, the length of the message and
 * the minimum of 8 bytes to send. The length field is the total length
 * of the data (including the 8 bytes) in bytes 
 */
typedef struct message_simple_s{
    struct message_simple_s * next;
    unsigned int from;
    size_t length;
    unsigned int data[2];
} message_simple_t;

/**
 * @brief The message info structure
 * 
 * This structure is stored in the process structure and contains all
 * information needed for message passing
 */
typedef struct{
	// Simple message
    message_simple_t * simple;
    unsigned int simple_number;
    unsigned int simple_recv_thread;
	// Simple message box
	message_simple_t * simple_box[256];
	unsigned int simple_box_number[256];
	unsigned int simple_box_recv_thread[256];
} message_info_t;

/**
 * @brief Initialize the message info structure of a process
 * 
 * @param info Address of the message info structure
 */
void message_init_info(message_info_t * info);

enum MESSAGE_SIMPLE_ERROR{
    MESSAGE_SIMPLE_ERROR_NOERROR = 0,
    MESSAGE_SIMPLE_ERROR_MESSAGE_TO_SHORT,
    MESSAGE_SIMPLE_ERROR_TO_NONEXIST,
    MESSAGE_SIMPLE_ERROR_BUFFER_TO_SHORT,
    MESSAGE_SIMPLE_ERROR_NO_MESSAGES,
    MESSAGE_SIMPLE_ERROR_ALREADY_BLOCKED_RECEIVE
};

/**
 * @brief Send a simple message
 * 
 * @param to PID of receiver
 * @param length Length of the message, must be at least 8 bytes
 * @param data Pointer to the data to be send
 * @return Zero if successfull
 */
unsigned int message_simple_send(unsigned int to, size_t length, void * data);

/**
 * @brief Receive a simple message
 * 
 * @param buffer Pointer to large enough receive buffer
 * @param length Length of receive buffer
 * @param from Pointer to place to store sender PID
 * @return Zero if successfull
 */
unsigned int message_simple_receive(void * buffer, size_t * length, unsigned int * from);

/**
 * @brief Receive a simple message blocking the thread
 * 
 * @param buffer Pointer to large enough receive buffer
 * @param length Length of receive buffer
 * @param from Pointer to place to store sender PID
 * @return Zero if successfull
 */
unsigned int message_simple_receive_blocking(void * buffer, size_t * length, unsigned int * from);

/**
 * @brief Send a simple message to a box
 * 
 * @param to PID of receiver
 * @param length Length of the message, must be at least 8 bytes
 * @param data Pointer to the data to be send
 * @param box Message box
 * @return Zero if successfull
 */
unsigned int message_simple_box_send(unsigned int to, size_t length, void * data, unsigned int box);

/**
 * @brief Receive a simple message from a box
 * 
 * @param buffer Pointer to large enough receive buffer
 * @param length Length of receive buffer
 * @param from Pointer to place to store sender PID
 * @param box Message box
 * @return Zero if successfull
 */
unsigned int message_simple_box_receive(void * buffer, size_t * length, unsigned int * from, unsigned int box);

/**
 * @brief Receive a simple message from a box blocking the thread
 * 
 * @param buffer Pointer to large enough receive buffer
 * @param length Length of receive buffer
 * @param from Pointer to place to store sender PID
 * @param box Message box
 * @return Zero if successfull
 */
unsigned int message_simple_box_receive_blocking(void * buffer, size_t * length, unsigned int * from, unsigned int box);

#endif
