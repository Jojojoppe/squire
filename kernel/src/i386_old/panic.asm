bits 32

; INCLUDES
; --------
%include "serial.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------
S_00			db 0x0a, 0x0d
				db "KERNEL PANIC: ", 0
S_11			db "-------------", 0x0a, 0x0d,
				db "error code: ", 0
S_12			db 0x0a, 0x0d
				db "kernel panic: ", 0
S_01			db 0x0a, 0x0d, "eax: ", 0
S_02			db "  ebx: ", 0
S_03			db 0x0a, 0x0d, "ecx: ", 0
S_04			db "  edx: ", 0
S_05			db 0x0a, 0x0d, "esi: ", 0
S_06			db "  edi: ", 0
S_07			db 0x0a, 0x0d, "esp: ", 0
S_08			db "  ebp: ", 0
S_09			db 0x0a, 0x0d, "eip: ", 0
S_10			db "  cr2: ", 0
S_13			db 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Panic function
;	[0]:	error code
; --------------
global panic
panic:
		push	ebp
		mov		ebp, esp
		cli

		mov		eax, S_00
		call	serial_outs
		mov		eax, [ebp+10*4]
		call	serial_outs
		mov		eax, S_11
		call	serial_outs
		; Get error code
		mov		eax, [ebp+13*4]
		call	serial_outhex

		; Print registers
		; eax, ebx, ecx, edx
		mov		eax, S_01
		call	serial_outs
		mov		eax, [ebp+9*4]
		call	serial_outhex
		mov		eax, S_02
		call	serial_outs
		mov		eax, [ebp+6*4]
		call	serial_outhex
		mov		eax, S_03
		call	serial_outs
		mov		eax, [ebp+8*4]
		call	serial_outhex
		mov		eax, S_04
		call	serial_outs
		mov		eax, [ebp+7*4]
		call	serial_outhex
		; edi, esi
		mov		eax, S_05
		call	serial_outs
		mov		eax, [ebp+3*4]
		call	serial_outhex
		mov		eax, S_06
		call	serial_outs
		mov		eax, [ebp+2*4]
		call	serial_outhex
		; esp, ebp
		mov		eax, S_07
		call	serial_outs
		mov		eax, [ebp+5*4]
		call	serial_outhex
		mov		eax, S_08
		call	serial_outs
		mov		eax, [ebp+4*4]
		call	serial_outhex
		; eip, cr2
		mov		eax, S_09
		call	serial_outs
		mov		eax, [ebp+11*4]
		call	serial_outhex
		mov		eax, S_10
		call	serial_outs
		mov		eax, [ebp+12*4]
		call	serial_outhex
		
		mov		eax, S_13
		call	serial_outs

.hang:
		cli
		hlt
		jmp		.hang

; Soft panic function
;	[0]:	error code
; -------------------
global panic_soft
panic_soft:
		push	ebp
		mov		ebp, esp
		cli

		mov		eax, S_12
		call	serial_outs
		mov		eax, [ebp+10*4]
		call	serial_outs
		mov		eax, S_11
		call	serial_outs
		; Get error code
		mov		eax, [ebp+13*4]
		call	serial_outhex

		; Print registers
		; eax, ebx, ecx, edx
		mov		eax, S_01
		call	serial_outs
		mov		eax, [ebp+9*4]
		call	serial_outhex
		mov		eax, S_02
		call	serial_outs
		mov		eax, [ebp+6*4]
		call	serial_outhex
		mov		eax, S_03
		call	serial_outs
		mov		eax, [ebp+8*4]
		call	serial_outhex
		mov		eax, S_04
		call	serial_outs
		mov		eax, [ebp+7*4]
		call	serial_outhex
		; edi, esi
		mov		eax, S_05
		call	serial_outs
		mov		eax, [ebp+3*4]
		call	serial_outhex
		mov		eax, S_06
		call	serial_outs
		mov		eax, [ebp+2*4]
		call	serial_outhex
		; esp, ebp
		mov		eax, S_07
		call	serial_outs
		mov		eax, [ebp+5*4]
		call	serial_outhex
		mov		eax, S_08
		call	serial_outs
		mov		eax, [ebp+4*4]
		call	serial_outhex
		; eip, cr2
		mov		eax, S_09
		call	serial_outs
		mov		eax, [ebp+11*4]
		call	serial_outhex
		mov		eax, S_10
		call	serial_outs
		mov		eax, [ebp+12*4]
		call	serial_outhex
		
		mov		eax, S_13
		call	serial_outs

		mov		esp, ebp
		pop		ebp
		ret