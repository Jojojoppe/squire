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

sc_receive		resb 256

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

		; Check for message
		mov		edx, sc_receive
		mov		ecx, 256
		mov		eax, 33
		int		0x80

		mov		eax, sc_receive
		mov		edx, 256
		int		0x82

		mov		eax, sc_receive+12
		int		0x81

.lp0:
		jmp		.lp0
