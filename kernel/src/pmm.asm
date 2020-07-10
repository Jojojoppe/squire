bits 32

; INCLUDES
; --------
%include "mboot.inc"
%include "serial.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------
S_FREE					db "Free memory (bytes): ", 0
S_RN					db 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; Physical memory bitmap
; bit=1 means free
pmm_map_bottom			resb 0x20000
pmm_map_top:

pmm_used				resd 1
pmm_free				resd 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize pmm
;	eax:	address of mboot header
; --------------
global pmm_init
pmm_init:
		push	ebp
		mov		ebp, esp
		sub		esp, 4		; -4:	mboot header address
		mov		[ebp-4], eax

		mov		eax, 0
.lp:
		; eax = last mmap field
		mov		edx, [ebp-4]		; Get mboot info header address
		call	mboot_get_mmap
		push	eax
		push	edx
		push	ecx					; Need these registers for next round
		; Check if last region
		test	eax, eax
		jz		.last
		; Check if usable
		test	ecx, ecx
		jz		.next

		shr		edx, 12
		shr		ecx, 12
		; edx = region page number
		; ecx = region size in pages
		mov		eax, edx
		mov		edx, ecx
		call	pmm_unuse


		; To next region
.next:
		pop		ecx
		pop		edx
		pop		eax
		jmp		.lp

.last:
		; Cleanup stack
		add		esp, 12

		mov		eax, S_FREE
		call	serial_outs
		mov		eax, [pmm_free]
		call	serial_outdec
		mov		eax, S_RN
		call	serial_outs

		mov		esp, ebp
		pop		ebp
		ret

; Unuse address region
;	eax:	page number
;	edx:	amount of pages
; --------------------
global pmm_unuse
pmm_unuse:
		push	ebp
		mov		ebp, esp
		sub		esp, 8		; -4:	page number
							; -8:	amount of pages
		mov		[ebp-4], eax
		mov		[ebp-8], edx

		mov		ecx, edx
.lp:
		mov		edx, [ebp-4]
		mov		eax, edx
		; Get byte number of page
		shr		eax, 5
		; Get address of byte
		add		eax, pmm_map_bottom
		; get bit number of page in byte
		and		edx, 0x1f
		bts		[eax], edx
		add		dword [pmm_free], 4096
		; To next page
		inc		dword [ebp-4]
		dec		ecx
		jnz		.lp

		mov		esp, ebp
		pop		ebp
		ret
