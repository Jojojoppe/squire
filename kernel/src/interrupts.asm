bits 32

; INCLUDES
; --------
%include "idt.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize interrupts
; ---------------------
global interrupts_init
interrupts_init:
		push	ebp
		mov		ebp, esp


		mov		esp, ebp
		pop		ebp
		ret
