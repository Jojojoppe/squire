#ifndef __H_ARCH_USER_IO
#define __H_ARCH_USER_IO

/**
 * @brief Check if ISR may be registerd
 *
 * @param id The id or number of the interrupt
 * @return int Zero if registration is allowed
 */
int user_io_arch_may_register_isr(unsigned int id);

/**
 * @brief Register ISR for a process
 *
 * @param id The id or number of the interrupt
 * @param PID The PID of the process
 */
void user_io_arch_register_isr(unsigned int id, unsigned int PID);

/**
 * @brief Check if IO access on port is allowed
 *
 * @param port The port to check access to
 * @param flags R/W flag. bit 0 for read bit 1 for write
 * @return int Zero if registration is allowed
 */
int user_io_arch_may_register_port(unsigned int port, unsigned int flags);

/**
 * @brief Register IO port access
 *
 * @param port The port to register
 * @param flags R/W flag. bit 0 for read bit 1 for write
 * @param PID PID of the process
 */
void user_io_arch_register_port(unsigned int port, unsigned int flags, unsigned int PID);

// The IO port functions
void user_io_arch_outb(unsigned int address, unsigned char val);
void user_io_arch_outw(unsigned int address, unsigned short val);
void user_io_arch_outd(unsigned int address, unsigned int val);
unsigned char user_io_arch_inb(unsigned int address);
unsigned short user_io_arch_inw(unsigned int address);
unsigned int user_io_arch_ind(unsigned int address);

#endif
