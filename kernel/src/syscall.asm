bits 32

; INCLUDES
; --------
%include "serial.inc"
%include "proc.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------

S_00			db "SYSCALL", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Syscall ISR
; -----------
global isr_syscall
isr_syscall:
		pushad
		cli

		sti
		popad
		iret

; Debug syscall ISR
; -----------
global isr_syscall_debug
isr_syscall_debug:
		pushad
		cli

		; Check if address is valid
		cmp		eax, 0xc0000000
		jnl		.end
		cmp		eax, 0x00400000
		jnge	.end
		call	serial_outs
		
.end:
		call	proc_schedule
		sti
		popad
		iret
