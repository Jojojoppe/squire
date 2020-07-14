bits 32

; ------------
; SECTION DATA
section .data
; ------------
S_00			db "This is init!", 0x0a, 0x0d, 0

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
.lp:
		mov		eax, S_00
		int		0x81
		jmp		.lp
