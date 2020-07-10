bits 32

; INCLUDES
; --------
%include "pmm.inc"
%include "serial.inc"
; --------

%define KERNEL_PT				0xffc00000
%define KERNEL_PD				0xfffff000

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

; Initialize VAS
; --------------
global vas_init
vas_init:
		push	ebp
		mov		ebp, esp

		; Test map to kernel space
		mov		eax, 0
		mov		edx, 0xc0000000
		call	vas_map

		mov		esp, ebp
		pop		ebp
		ret

; Map physical to VAS
;	eax:	physical address
;	edx:	virtual address
; -------------------
global vas_map
vas_map:
		push	ebp
		mov		ebp, esp
		sub		esp, 24		; -4:	physical address
							; -8:	virtual address
							; -12:	PT
							; -16:	PD
							; -24:	PDE
							; -28:	PTE
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		; Get PD and PT
		shr		edx, 12
		mov		[ebp-12], edx
		shr		edx, 10
		mov		[ebp-16], edx
		; Get PDE
		shl		edx, 2
		add		edx, KERNEL_PD
		mov		eax, [edx]
		mov		[ebp-24], eax
		; eax contains PDE

		; Check if there is a used entry
		bt		eax, 0
		jnc		.create_pd
		; There is a used entry
		; Check if it is a 4MB page, if so it must be split
		bt		eax, 7
		jc		.split_pd

.pd_ok:
		int 0

		mov		esp, ebp
		pop		ebp
		ret

.create_pd:
		; Get PD and create it
		mov		eax, [ebp-16]
		call	_vas_create_pd
		jmp		.pd_ok
.split_pd:
		; Get PD and split
		mov		eax, [ebp-16]
		call	_vas_split
		jmp		.pd_ok

; Split 4MB page
;	eax:	PD
; --------------
_vas_split:
		push	ebp
		mov		ebp, esp
		sub		esp, 16		; -4:	PD
							; -8:	PT physical page
							; -12:	physical address
							; -16:	ebx
		mov		[ebp-4], eax
		mov		[ebp-16], ebx
		; TODO create _vas_split

		; Get a physical page to use as PT
		call	pmm_alloc
		mov		[ebp-8], eax

		; Get PDE
		mov		edx, [ebp-4]
		shl		edx, 2
		add		edx, KERNEL_PD
		mov		eax, [edx]
		; eax contains PDE
		and		eax, 0xfffff000
		mov		[ebp-12], eax
		; Map new PT in temporary PD (directory 0) to be written
		mov		eax, [ebp-8]
		or		eax, 0x83			; Present and R/W
		mov		[KERNEL_PD], eax

		; Loop over all 1024 entries to map full 4MB
		mov		ecx, 1024
		mov		ebx, [ebp-12]
		mov		edx, 0

.lp:
		; Write PTE
		mov		eax, ebx
		or		eax, 0x03			; Present and R/R
		mov		[edx], eax
		; To next PTE
		add		edx, 4
		add		ebx, 4096  
		dec		ecx
		jnz		.lp

		; Map newly written PT to original PD
		mov		edx, [ebp-4]
		shl		edx, 2
		add		edx, KERNEL_PD
		mov		eax, [ebp-8]
		or		eax, 0x03
		mov		[edx], eax
		; Unmap temporary PDE
		xor		eax, eax
		mov		[KERNEL_PD], eax

		mov		esp, ebp
		pop		ebp
		ret

; Create a PD
;	eax:	PD
; -----------
_vas_create_pd:
		push	ebp
		mov		ebp, esp
		; TODO create _vas_create_pd
		mov		esp, ebp
		pop		ebp
		ret
