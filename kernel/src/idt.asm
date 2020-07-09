bits 32

; ------------
; SECTION DATA
section .data
align 0x04
; ------------

global idtr
idtr:
		dw		idt_end-idt_base	; Size
		dd		idt_base		; Offset

; -----------
; SECTION BSS
section .bss
align 0x04
; -----------

; IDT
; ---
global idt_base
idt_base								resb 0x2000
idt_end:	

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Install idt
; -----------
global idt_install
idt_install:
		push	ebp
		mov		ebp, esp


		mov		esp, ebp
		pop		ebp
		ret
