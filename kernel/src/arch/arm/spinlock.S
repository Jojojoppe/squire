// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

// void arch_spinlock_init(spinlock_t * lock)
// TODO implement
// ------------------------------------------
.global arch_spinlock_init
arch_spinlock_init:
		bx		lr

// void arch_spinlock_lock(spinlock_t * lock)
// TODO implement
// ------------------------------------------
.global arch_spinlock_lock
arch_spinlock_lock:
		bx		lr

// void arch_spinlock_unlock(spinlock_t * lock)
// TODO implement
// --------------------------------------------
.global arch_spinlock_unlock
arch_spinlock_unlock:
		bx		lr
