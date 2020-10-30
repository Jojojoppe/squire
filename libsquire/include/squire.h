#ifndef __H_SQUIRE
#define __H_SQUIRE 1

#include <stddef.h>

#include <squire_syscall_mmap_.h>
#include <squire_syscall_procthread_.h>
#include <squire_syscall_simple_.h>
#include <squire_syscall_log_.h>
#include <squire_syscall_mutex_.h>

typedef unsigned int squire_spinlock_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern void squire_spinlock_init(squire_spinlock_t * lock);
extern void squire_spinlock_lock(squire_spinlock_t * lock);
extern void squire_spinlock_unlock(squire_spinlock_t * unlock);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
