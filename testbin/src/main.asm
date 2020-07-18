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

sc_log		resd 2

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Entry point
; -----------
global main
main:
		push	ebp
		mov		ebp, esp

		extern write
		mov		eax,17
		push	eax
		mov		eax, S_00
		push	eax
		mov		eax, 0
		push	eax
		call	write

.lp0:
		jmp		.lp0
