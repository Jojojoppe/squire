bits 32

; ------------
; SECTION DATA
section .data
align 0x1000
; ------------

; -----------
; SECTION BSS
section .bss
align 0x1000
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Entry point
; -----------
global g_start
g_start:
		cli
		jmp $
