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
#include <general/timer.h>

/**
 * @brief Initialize timer
 * 
 * @return zero if successful
 */
int timer_init();

#endif