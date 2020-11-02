.section .text

.globl isr_timer
isr_timer:
        pusha
        pushf

        call isr_c_timer

        popf
        popa
        sti
        iret

.globl isr_syscall
isr_syscall:
        pusha
        pushf

        call isr_c_syscall

        popf
        popa
        sti
        iret

get_eip:
		mov (%esp), %eax
		ret

.globl isr_user
.globl isr_user_start
isr_user:
isr_user_start:
		pusha
		pushf
		# The interrupt ID
		mov		$-1, %eax
		push	%eax
		# The PID
		mov		$0, %eax
		push	%eax
		lea		isr_c_user, %eax
		call	*%eax
		add		$8, %esp
		popf
		popa
		sti
		iret
.globl isr_user_end
isr_user_end:
