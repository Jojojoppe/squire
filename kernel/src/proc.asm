bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "vmm.inc"
; --------

struc process
	.id			resd 1				; Process ID
	.vmm		resd 1				; Memory regions list address
endstruc

; ------------
; SECTION DATA
section .data
; ------------

; -----------
; SECTION BSS
section .bss
align 0x04
; -----------

; Task state segment
; ------------------
TSS				resd 26

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize processes
; --------------------
global proc_init
proc_init
		push	ebp
		mov		ebp, esp

		; Initialize TSS
		mov		eax, TSS
		call	gdt_settss
		mov		edx, TSS
		mov		dword [edx+1*4], 0			; ESP0
		mov		dword [edx+2*4], 0x10		; SS0
		mov		dword [edx+25*4], 104		; IOPB

		; Test VMM
		call	vmm_create
		push	eax
		call	vmm_debugprint
		pop		edi
		push	edi
		mov		eax, 0x400000
		mov		edx, 0x4000
		mov		ecx, 1
		call	vmm_alloc
		pop		eax
		call	vmm_debugprint

		mov		esp, ebp
		pop		ebp
		ret
