// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

// void arch_spinlock_init(spinlock_t * lock)
// ------------------------------------------
.global arch_spinlock_init
arch_spinlock_init:
		movl	$0, %eax
		mov		%eax, (%rdi)
		ret

// void arch_spinlock_lock(spinlock_t * lock)
// ------------------------------------------
.global arch_spinlock_lock
arch_spinlock_lock:
		movl	$1, %ecx
arch_spinlock_lock.loop:
		movl	(%rdi), %eax
		test	%eax, %eax
		jnz		arch_spinlock_lock.loop
lock	cmpxchgl %ecx, (%rdi)
		jnz		arch_spinlock_lock.loop
		ret

// void arch_spinlock_unlock(spinlock_t * lock)
// --------------------------------------------
.global arch_spinlock_unlock
arch_spinlock_unlock:
		movl	$0, (%rdi)
		ret
