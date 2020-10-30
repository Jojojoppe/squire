# void squire_spinlock_init(squire_spinlock_t * lock);
.globl squire_spinlock_init
squire_spinlock_init:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		movl	$0, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret

# void squire_spinlock_lock(squire_spinlock_t * lock);
.globl squire_spinlock_lock
squire_spinlock_lock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx

squire_spinlock_lock.retry:
		mov		$1, %eax
		xchg	%eax, (%edx)
		test	%eax, %eax
		jnz		squire_spinlock_lock.retry

		mov		%ebp, %esp
		pop		%ebp
		ret

# void squire_spinlock_unlock(squire_spinlock_t * lock);
.globl squire_spinlock_unlock
squire_spinlock_unlock:
		push	%ebp
		mov		%esp, %ebp

		mov		8(%ebp), %edx
		xor		%eax, %eax
		xchg	%eax, (%edx)

		mov		%ebp, %esp
		pop		%ebp
		ret
