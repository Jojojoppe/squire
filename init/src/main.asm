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
S_TESTBIN		db "testbin.bin", 0

; -----------
; SECTION BSS
section .bss
; -----------

sc_log			resd 2
sc_process		resd 2

pid_testbin		resd 1

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
		sub		esp, 12			; -4:	Address of initramfs.tar
								; -8:	Address of testbin.bin
								; -12:	Length of testbin.bin
		mov		[ebp-4], eax

		; Print S_00
		mov		edx, sc_log
		mov		dword [edx+4*0], S_00
		mov		dword [edx+4*1], 15
		mov		ecx, 8
		mov		eax, 0x10000000
		int		0x80

		; Get testbin.bin
		mov		eax, [ebp-4]
		mov		edx, S_TESTBIN
		call	tar_getfile
		mov		[ebp-8], eax
		mov		[ebp-12], edx
		test	eax, eax
		jz		.lp0

		; Load new process (testbin.bin)
		mov		edx, sc_process
		mov		eax, [ebp-8]
		mov		dword [edx+4*0], eax
		mov		eax, [ebp-12]
		mov		dword [edx+4*1], eax
		mov		ecx, 4*2
		mov		eax, 17
		int		0x80
		mov		edx, sc_process
		mov		eax, [edx+4*0]
		mov		[pid_testbin], eax

.lp0:
		jmp		.lp0
