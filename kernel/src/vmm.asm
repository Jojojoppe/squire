bits 32

; INCLUDES
; --------
%include "kmalloc.inc"
%include "pmm.inc"
%include "vas.inc"
%include "serial.inc"
; --------

struc vmmregion
	.next		resd 1		; Linked list next, NULL if none
	.prev		resd 1		; Linked list prev, NULL if none
	.base		resd 1		; Base of memory region
	.length		resd 1		; Length of memory region
	.shared		resd 1		; Address of shared memory descriptor. Valid if FLAGS_SHARED is set
	.flags		resd 1

	.sizeof:
endstruc

%define FLAGS_USED 0
%define FLAGS_SHARED 1
%define FLAGS_READ 2
%define FLAGS_WRITE 3
%define FLAGS_EXEC 4

; ------------
; SECTION DATA
section .data
; ------------

S_00			db "Region:", 0x0a, 0x0d, "  - base: ", 0
S_01			db 0x0a, 0x0d, "  - length: ", 0
S_02			db 0x0a, 0x0d, "  - flags: ", 0
S_RN			db 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
align 0x04
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Create region list
;	->eax:	address of first region block
; ------------------
global vmm_create
vmm_create:
		push	ebp
		mov		ebp, esp
		sub		esp, 4			; -4:	descriptor address

		; Process starts with all space available from 4MiB to 3GiB
		; Allocate space for descriptor
		mov		eax, vmmregion.sizeof
		call	kmalloc
		mov		edx, eax
		mov		[ebp-4], eax
		; Set descriptor fields
		mov		dword [edx+vmmregion.next], 0
		mov		dword [edx+vmmregion.prev], 0
		mov		dword [edx+vmmregion.base], 0x400000		; 4MiB
		mov		dword [edx+vmmregion.length], 0xc0000000-0x400000
		mov		dword [edx+vmmregion.flags], 0

		mov		eax, edx
		mov		esp, ebp
		pop		ebp
		ret

; Destroy region list
; Frees all allocated memory and checks if shared memory should be
; freed as well
;	eax:	address of first region block
; -------------------
global vmm_destroy
vmm_destroy:
		push	ebp
		mov		ebp, esp
		sub		esp, 8			; -4:	descriptor list address
								; -8:	edi
		mov		[ebp-4], eax
		mov		[ebp-8], edi

		mov		edx, eax
.traverse:
		; Check if a used region
		mov		eax, [edx+vmmregion.flags]
		bt		eax, FLAGS_USED
		jnc		.next
		; Used region
		mov		edi, [edx+vmmregion.base]
		mov		ecx, [edx+vmmregion.length]
		shr		ecx, 12
.freelp:
		; TODO check for shared region
		; Free a mapped page
		mov		eax, edi
		call	vas_unmap
		; eax contains physical address to free
		mov		edx, 1
		call	pmm_unuse
		dec		ecx
		jnz		.freelp
.next:
		; Check if there is a next region
		mov		eax, edx
		mov		edx, [edx+vmmregion.next]
		push	edx
		; Free used memory for descriptor
		call	kfree
		pop		edx
		test	edx, edx
		jnz		.traverse
		; No next region: done

		mov		edi, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret

; Allocage memory region
;	eax:	address base to allocate to
;	edx:	length of region
;	ecx:	flags (R/W/E)
;	edi:	address of base list
;	->eax:	NULL if succeeded
; ----------------------
global vmm_alloc
vmm_alloc:
		push	ebp
		mov		ebp, esp
		sub		esp, 20			; -4:	address base to allocate to
								; -8:	length of region
								; -12:	flags
								; -16:	ebx
								; -20:	Address of base list
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		mov		[ebp-16], ebx
		mov		[ebp-20], edi

		; Find descriptor of region (start)
.traverse:
		; Check if destination base is within this descriptor
		mov		eax, [ebp-4]
		push	edx
		mov		edx, [edi+vmmregion.base]
		pop		edx
		cmp		eax, [edi+vmmregion.base]
		; If destination base is not larger than base of region destination is not within region
		jnae	.next
		; Check if destination is smaller than end of region
		mov		edx, [edi+vmmregion.base]
		add		edx, [edi+vmmregion.length]
		cmp		eax, edx
		jnb		.next
		; Destination starts within current region
		; Check if already used
		mov		eax, [edi+vmmregion.flags]
		bt		eax, FLAGS_USED
		jc		.enderror
		; Region is not used
		; Check if end is within region bounds
		mov		eax, [ebp-8]
		cmp		[edi+vmmregion.length], eax
		jbe		.enderror
		; Region to allocate fits insize memory region

		; Check if a new region must be made before destination
		mov		eax, [ebp-4]
		mov		edx, [edi+vmmregion.base]
		cmp		eax, [edi+vmmregion.base]
		jna		.aftercreatestart
		; New region must be constructed before destination
		; Allocate space for descriptor
		mov		eax, vmmregion.sizeof
		call	kmalloc
		mov		edx, eax
		; Link in list after current descriptor
		; Set next field of new
		mov		eax, [edi+vmmregion.next]
		mov		[edx+vmmregion.next], eax
		; Set prev field of new
		mov		[edx+vmmregion.prev], edi
		; Set next field of current
		mov		[edi+vmmregion.next], edx
		; Check if need to set prev of currents next descriptor
		mov		eax, [edx+vmmregion.next]
		test	eax, eax
		jz		.aftersetprev
		mov		[eax+vmmregion.prev], edx
.aftersetprev:
		; Descriptor is linked in list
		; Set fields of new and current
		mov		eax, [ebp-4]
		mov		[edx+vmmregion.base], eax
		mov		eax, [edi+vmmregion.flags]
		mov		[edx+vmmregion.flags], eax
		mov		ebx, [ebp-4]
		sub		ebx, [edi+vmmregion.base]		; ebx is length of current region left
		mov		eax, [edi+vmmregion.length]
		sub		eax, ebx			; eax is lenght of new region
		mov		[edi+vmmregion.length], ebx
		mov		[edx+vmmregion.length], eax
		mov		edi, edx
.aftercreatestart:
		; edi contains descriptor of usable region
		; Check if its needed to create region after new
		mov		eax, [edi+vmmregion.length]
		cmp		[ebp-8], eax
		jnb		.aftercreateend
		; Allocate space for descriptor
		mov		eax, vmmregion.sizeof
		call	kmalloc
		mov		edx, eax
		; Link in list after current descriptor
		; Set next field of new
		mov		eax, [edi+vmmregion.next]
		mov		[edx+vmmregion.next], eax
		; Set prev field of new
		mov		[edx+vmmregion.prev], edi
		; Set next field of current
		mov		[edi+vmmregion.next], edx
		; Check if need to set prev of currents next descriptor
		mov		eax, [edx+vmmregion.next]
		test	eax, eax
		jz		.aftersetprev2
		mov		[eax+vmmregion.prev], edx
.aftersetprev2:
		; Descriptor is in linked list
		; Set fields of new and current
		mov		eax, [ebp-4]
		add		eax, [ebp-8]
		mov		[edx+vmmregion.base], eax
		mov		eax, [edi+vmmregion.flags]
		mov		[edx+vmmregion.flags], eax
		mov		ebx, [edx+vmmregion.base]
		sub		ebx, [edi+vmmregion.base]
		mov		eax, [edi+vmmregion.length]
		sub		eax, ebx
		mov		[edi+vmmregion.length], ebx
		mov		[edx+vmmregion.length], eax
.aftercreateend:
		; Region descriptor correctly inserted in list
		; Set flags of region
		mov		eax, [ebp-12]
		or		eax, 1<<FLAGS_USED
		mov		[edi+vmmregion.flags], eax
		; Allocate physical memory
		mov		ecx, [edi+vmmregion.length]
		shr		ecx, 12
		mov		edx, [edi+vmmregion.base]
.alloclp:
		push	ecx
		push	edx
		call	pmm_alloc
		pop		edx
		push	edx
		call	vas_map
		pop		edx
		pop		ecx

		add		edx, 4096
		dec		ecx
		jnz		.alloclp
		jmp		.end
.next:
		; Check if there is a next region
		mov		edi, [edi+vmmregion.next]
		test	edi, edi
		jnz		.traverse
		; No next region
		; Allocation not possible
.enderror:
		mov		eax, -1
		mov		ebx, [ebp-16]
		mov		esp, ebp
		pop		ebp
		ret
.end:
		xor		eax, eax
		mov		ebx, [ebp-16]
		mov		esp, ebp
		pop		ebp
		ret

; Debug print regions
;	eax:	address of first descriptor
; -------------------
global vmm_debugprint
vmm_debugprint:
		push	ebp
		mov		ebp, esp
		sub		esp, 8			; -4:	descriptor list address
								; -8:	edi
		mov		[ebp-4], eax
		mov		[ebp-8], edi

		mov		edx, eax
.traverse:
		mov		[ebp-4], edx

		mov		eax, S_00
		call	serial_outs
		mov		edx, [ebp-4]
		mov		eax, [edx+vmmregion.base]
		call	serial_outhex

		mov		eax, S_01
		call	serial_outs
		mov		edx, [ebp-4]
		mov		eax, [edx+vmmregion.length]
		call	serial_outhex

		mov		eax, S_02
		call	serial_outs
		mov		edx, [ebp-4]
		mov		eax, [edx+vmmregion.flags]
		call	serial_outhex

		mov		eax, S_RN
		call	serial_outs
.next:
		mov		edx, [ebp-4]
		mov		edx, [edx+vmmregion.next]
		test	edx, edx
		jnz		.traverse
		; No next region: done

		mov		edi, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret
