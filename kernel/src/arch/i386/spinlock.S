// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

// void arch_spinlock_init(spinlock_t * lock)
// ------------------------------------------
.global arch_spinlock_init
arch_spinlock_init:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		movl	$0, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret

// void arch_spinlock_lock(spinlock_t * lock)
// ------------------------------------------
.global arch_spinlock_lock
arch_spinlock_lock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
arch_spinlock_lock.retry:
		mov		$1, %eax
		xchg	%eax, (%edx)
		test	%eax, %eax
		jnz		arch_spinlock_lock.retry

		mov		%ebp, %esp
		pop		%ebp
		ret

// void arch_spinlock_unlock(spinlock_t * lock)
// --------------------------------------------
.global arch_spinlock_unlock
arch_spinlock_unlock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		xor		%eax, %eax
		xchg	%eax, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret
