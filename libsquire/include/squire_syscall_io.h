#ifndef __H_SQUIRE_SYSCALL_IO
#define __H_SQUIRE_SYSCALL_IO 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

/**
 * @brief IO operations
 * 
 */
typedef enum SQUIRE_SYSCALL_IO_OPERATION{
	/**
	 * @brief Register ISR
	 *
	 * Registers an interrupt handler which will cause a SIGINTR with
	 * squire_extraval0 containing the interrupt id. The id is architecture
	 * dependent. On x86 cpu's the id is the same as the interrupt number.
	 * Squire may reject the registration for any reason (already in use, 
	 * non-registerble or not supported).
	 * value0:		The interrupt id or number to register
	 * After execution return0 contains the status of the registration: Zero
	 * for success and non-zero for error (TODO assign values for errors)
	 */
	SQUIRE_SYSCALL_IO_OPERATION_REGISTER_ISR
} squire_syscall_io_operation_t;

/**
 * @brief Parameter structure for IO operation
 * 
 */
typedef struct{
    squire_syscall_io_operation_t           operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
	unsigned int							value0;
	int										return0;
} squire_syscall_io_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif



#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif
