#ifndef __H_ARCH_SPINLOCK
#define __H_ARCH_SPINLOCK 1

typedef unsigned int spinlock_t;

extern void spinlock_init(spinlock_t * lock);
extern void spinlock_lock(spinlock_t * lock);
extern void spinlock_unlock(spinlock_t * unlock);

#endif
