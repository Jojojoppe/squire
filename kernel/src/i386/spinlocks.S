# void spinlock_init(spinlock_t * lock);
.globl spinlock_init
spinlock_init:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		movl	$0, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret

# void spinlock_lock(spinlock_t * lock);
.globl spinlock_lock
spinlock_lock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx

spinlock_lock.retry:
		mov		$1, %eax
		xchg	%eax, (%edx)
		test	%eax, %eax
		jnz		spinlock_lock.retry

		mov		%ebp, %esp
		pop		%ebp
		ret

# void spinlock_unlock(spinlock_t * lock);
.globl spinlock_unlock
spinlock_unlock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		xor		%eax, %eax
		xchg	%eax, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret
