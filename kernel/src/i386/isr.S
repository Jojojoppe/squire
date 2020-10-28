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
