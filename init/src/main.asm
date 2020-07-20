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

pid_testbin		resd 1

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
		sub		esp, 12			; -4:	Address of initramfs.tar
								; -8:	Address of testbin.bin
								; -12:	Length of testbin.bin
		mov		[ebp-4], eax

		; Print S_00
		extern printf
		mov		eax, S_00
		push	eax
		call	printf

		; Get testbin.bin
		mov		eax, [ebp-4]
		mov		edx, S_TESTBIN
		call	tar_getfile
		mov		[ebp-8], eax
		mov		[ebp-12], edx
		test	eax, eax
		jz		.lp0

		; Load new process (testbin.bin)
		extern squire_syscall_process
		mov		eax, [ebp-12]
		push	eax
		mov		eax, [ebp-8]
		push	eax
		call	squire_syscall_process
		mov		[pid_testbin], eax

.lp0:
		jmp		.lp0
