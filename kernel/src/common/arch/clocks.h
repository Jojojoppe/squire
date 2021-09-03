#ifndef __H_COMMON_ARCH_CLOCKS
#define __H_COMMON_ARCH_CLOCKS 1

#include <stddef.h>

/*
 * Initialize system clock and install interrupt
 */
extern void arch_sysclock_init();

/*
 * Enable system clock interrupt
 */
extern void arch_sysclock_enable();

/*
 * Disable system clock interrupt
 */
extern void arch_sysclock_disable();

#endif