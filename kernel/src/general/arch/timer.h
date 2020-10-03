#ifndef __H_ARCH_TIMER
#define __H_ARCH_TIMER 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent timer
 */

#include <general/config.h>
#include <general/stdint.h>
#include <general/vmm.h>

/**
 * @brief Initialize timer
 * 
 * @return zero if successful
 */
int timer_init();

/**
 * @brief Timer interrupt handler
 * 
 */
void timer_interrupt();

/**
 * @brief Get timer value
 * 
 * @return timer value
 */
unsigned int timer_get();

#endif