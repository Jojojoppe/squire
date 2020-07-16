bits 32

; INCLUDES
; --------
%include "tar.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------
S_00			db "This is init!", 0x0a, 0x0d, 0

S_01			db "A", 0x0a, 0x0d, 0
S_02			db "B", 0x0a, 0x0d, 0

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
		sub		esp, 4			; -4:	Address of initramfs.tar
		mov		[ebp-4], eax

		mov		eax, S_00
		int		0x81

		mov		eax, [ebp-4]
		call	tar_printlist

.lp0:
		jmp		.lp0
