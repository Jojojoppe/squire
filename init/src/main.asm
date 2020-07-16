bits 32

; ------------
; SECTION DATA
section .data
; ------------
S_00			db "This is init!", 0x0a, 0x0d, 0

S_01			db "A", 0
S_02			db "B", 0

; -----------
; SECTION BSS
section .bss
; -----------

sc_mmap			resd 3
sc_thread		resd 3

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

		; Create user stack
		mov		eax, 1
		mov		edx, sc_mmap
		mov		dword [edx+0*4], 0x10000000	; Address
		mov		dword [edx+1*4], 4096		; Length
		mov		dword [edx+2*4], 0			; Flags
		mov		ecx, 3*4
		int		0x80

		; Create user thread
		mov		eax, 16
		mov		edx, sc_thread
		mov		dword [edx+0*4], .lp1
		mov		dword [edx+1*4], 0x10000000 + 4096 - 4
		mov		dword [edx+2*4], 0
		mov		ecx, 3*4
		int		0x80


.lp0:
		mov		eax, S_01
		int		0x81
		jmp		.lp0

.lp1:
		mov		eax, S_02
		int		0x81
		jmp		.lp1
