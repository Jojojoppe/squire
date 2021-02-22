#ifndef __H_SQUIRE_SYSCALL_IO
#define __H_SQUIRE_SYSCALL_IO 1

#ifndef __H_SQUIRE_SYSCALL
    #error Do not include squire_syscall_xxx.h files directly! Use squire.h or squire_syscall.h instead.
#else

#include <stddef.h>

// Operation defines
#define IO_PORT_READ 1
#define IO_PORT_WRITE 2

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
	SQUIRE_SYSCALL_IO_OPERATION_REGISTER_ISR,
	/**
	 * @brief Register IO port
	 *
	 * Registers an IO port for use. Flags contain R/W access status
	 * value0:		The port to register
	 * value1:		Range: amount of ports from value0
	 * After execution return0 contains the status of the registration: Zero
	 * for success and non-zero for error
	 */
	SQUIRE_SYSCALL_IO_OPERATION_REGISTER_PORT,
	/**
	 * @brief Output byte to port
	 *
	 * value0:		The port
	 * value1:		The value to write
	 * After execution return0 contains status. Zero on success
	 */
	SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTB,
	SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTW,
	SQUIRE_SYSCALL_IO_OPERATION_PORT_OUTD,
	/**
	 * @brief Output byte from port
	 *
	 * value0:		The port
	 * After execution return0 contains status. Zero on success. value1
	 * contains the value from the port
	 */
	SQUIRE_SYSCALL_IO_OPERATION_PORT_INB,
	SQUIRE_SYSCALL_IO_OPERATION_PORT_INW,
	SQUIRE_SYSCALL_IO_OPERATION_PORT_IND,
} squire_syscall_io_operation_t;

/**
 * @brief Parameter structure for IO operation
 * 
 */
typedef struct{
    squire_syscall_io_operation_t           operation;      /** @brief opcode */
    int                                     flags;          /** @brief flags */
	unsigned int							value0;
	unsigned int							value1;
	int										return0;
} squire_syscall_io_t;

// SQUIRE SYSCALL WRAPPER FUNCTIONS
// --------------------------------
#if defined(__cplusplus)
extern "C" {
#endif

extern int squire_io_register_isr(unsigned int id);
extern int squire_io_register_port(unsigned int port, unsigned int range, unsigned int flags);
extern int squire_io_port_outb(unsigned int port, unsigned char val);
extern int squire_io_port_outw(unsigned int port, unsigned short val);
extern int squire_io_port_outd(unsigned int port, unsigned int val);
extern int squire_io_port_inb(unsigned int port, unsigned char * val);
extern int squire_io_port_inw(unsigned int port, unsigned short * val);
extern int squire_io_port_ind(unsigned int port, unsigned int * val);

#if defined(__cplusplus)
} /* extern "C" */
#endif
#endif
#endif
