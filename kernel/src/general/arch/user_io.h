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

#endif
