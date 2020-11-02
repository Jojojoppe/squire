#ifndef __H_USER_IO
#define __H_USER_IO 1

#include <general/arch/user_io.h>

typedef struct user_io_isr_registration_s{
	struct user_io_isr_registration_s * next;
	unsigned int id;
	unsigned int PID;
} user_io_isr_registration_t;

typedef struct user_io_port_registration_s{
	struct user_io_port_registration_s * next;
	unsigned int port;
	unsigned int range;
	unsigned int flags;
	unsigned int PID;
} user_io_port_registration_t;

/**
 * @brief Register ISR
 *
 * @param id The id or number of the interrupt
 * @param PID PID of the process
 * @return int Zero if registration is successful
 */
int user_io_register_isr(unsigned int id, unsigned int PID);

#define USER_IO_PORT_READ 1
#define USER_IO_PORT_WRITE 2

/**
 * @brief Register IO port
 *
 * @param port The port to register
 * @param range Amount of ports to register starting from 'port'
 * @param flags R/W flags: bit 0 R bit 1 W
 * @param PID PID of the process
 * @return int Zero if registration is succesfull
 */
int user_io_register_port(unsigned int port, unsigned int range, unsigned int flags, unsigned int PID);

int user_io_port_outb(unsigned int port, unsigned char val);
int user_io_port_inb(unsigned int port, unsigned char * val);

#endif
