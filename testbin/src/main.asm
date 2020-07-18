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
global _start
_start:
		push	ebp
		mov		ebp, esp

		; Print S_00
		mov		edx, sc_log
		mov		dword [edx+4*0], S_00
		mov		dword [edx+4*1], 17
		mov		ecx, 8
		mov		eax, 0x10000000
		int		0x80

.lp0:
		jmp		.lp0
