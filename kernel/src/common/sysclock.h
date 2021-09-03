#ifndef __H_COMMON_SYSCLOCK
#define __H_COMMON_SYSCLOCK 1

/*
 * Initialize system clock
 */
void sysclock_init();

/*
 * Enable system clock
 */
void sysclock_enable();

/*
 * Disable system clock
 */
void sysclock_disable();

/*
 * Callback for system clock interrupt
 */
void sysclock_cb();

#endif