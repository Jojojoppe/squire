#ifndef __H_USER_IO
#define __H_USER_IO 1

#include <general/arch/user_io.h>

/**
 * @brief Register ISR
 *
 * @param id The id or number of the interrupt
 * @return int Zero if registration is successful
 */
int user_io_register_isr(unsigned int id, unsigned int PID);

#endif
