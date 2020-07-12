bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "vmm.inc"
%include "kmalloc.inc"
; --------

struc process
	.next		resd 1
	.prev		resd 1

	.id			resd 1				; Process ID
	.memory		resd 1				; Memory regions list address
	.vas		resd 1				; CR3 for process
	.threads	resd 1				; Pointer to thread list

	.sizeof:
endstruc

struc thread
	.next		resd 1
	.prev		resd 1

	.id			resd 1
	.kstack		resd 1
	.ustack		resd 1

	.sizeof:
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
TSS					resd 26

; PID counter
proc_pidcounter		resd 1
; Current process
proc_proccurrent	resd 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize processes
; --------------------
global proc_init
proc_init:
		push	ebp
		mov		ebp, esp

		; Initialize TSS
		mov		eax, TSS
		call	gdt_settss
		mov		edx, TSS
		mov		dword [edx+1*4], 0			; ESP0
		mov		dword [edx+2*4], 0x10		; SS0
		mov		dword [edx+25*4], 104		; IOPB

		; Set PID counter
		mov		eax, 2
		mov		[proc_pidcounter], eax

		; Create main process structure
		mov		eax, process.sizeof
		call	kmalloc
		mov		[proc_proccurrent], eax
		; Fill process structure
		mov		edx, eax
		mov		[edx+process.next], edx
		mov		[edx+process.prev], edx
		mov		dword [edx+process.id], 1
		; Create memory region list
		call	vmm_create
		mov		dword [edx+process.memory], eax
		mov		eax, cr3
		mov		dword [edx+process.vas], eax
		; Create main thread structure
		mov		eax, thread.sizeof
		call	kmalloc
		mov		dword [edx+process.threads], eax
		; Fill tread structure
		mov		edx, eax
		mov		dword [edx+thread.next], 0
		mov		dword [edx+thread.prev], 0
		mov		dword [edx+thread.id], 1
		mov		dword [edx+thread.kstack], 0		; TODO set value
		mov		dword [edx+thread.ustack], 0		; TODO set value

		; TODO finish

		mov		esp, ebp
		pop		ebp
		ret
