bits 32

; ------------
; SECTION DATA
section .data
; ------------

S_00		db "This is testbin", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Entry point
; -----------
global _start
_start:
		push	ebp
		mov		ebp, esp

		mov		eax, S_00
		int		0x81

.lp0:
		jmp		.lp0
