#ifndef __H_COMMON_ARCH_SPINLOCK
#define __H_COMMON_ARCH_SPINLOCK 1

/*
 * General kernel spinlock
 */
typedef unsigned int arch_spinlock_t;

/*
 * Initialize spinlock
 */
extern void arch_spinlock_init(arch_spinlock_t * lock);

/*
 * Blocking locking a spinlock
 */
extern void arch_spinlock_lock(arch_spinlock_t * lock);

/*
 * Unblocking locking a spinlock
 * Returns zero if locked, non-zero if not locked
 * TODO implement spinlock_lock_unblocking
 */
//int spinlock_lock_unblocking(arch_spinlock_t * lock);

/*
 * Unlock a lock
 */
extern void arch_spinlock_unlock(arch_spinlock_t * lock);

#endif
