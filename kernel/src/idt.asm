bits 32

struc idtentry
	.offset_1			resw 1
	.selector			resw 1
	.zero				resb 1
	.type_attr			resb 1
	.offset_2			resw 1
endstruc

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

		mov		eax, idtr
		lidt	[eax]				; Load table pointer

		mov		esp, ebp
		pop		ebp
		ret

; Set interrupt gate
;	eax, interrupt number
;	edx, interrupt handler
; ------------------
global idt_set_interrupt
idt_set_interrupt:
		push	ebp
		mov		ebp, esp
		sub		esp, 8
		mov		[ebp-4], edi
		mov		[ebp-8], eax

		mov		edi, idt_base
		shl		eax, 3
		add		edi, eax			; eax = idtentry
		mov		[edi + idtentry.offset_1], cx
		rol		ecx, 16
		mov		[edi + idtentry.offset_2], cx
		mov		eax, 0x08
		mov		[edi + idtentry.selector], ax
		xor		eax, eax
		mov		[edi + idtentry.zero], al
		mov		eax, 0x8e
		mov		[edi + idtentry.type_attr], al

		mov		edi, [ebp-4]
		mov		eax, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret
