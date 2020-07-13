bits 32

; INCLUDES
; --------
%include "serial.inc"
%include "proc.inc"
%include "vmm.inc"
; --------

struc elfheader
	.magic		resd 1
	.mode		resb 1
	.endianness	resb 1
	.version	resb 1
	.ABI		resb 1
	.unused		resb 8
	.type		resw 1
	.ISA		resw 1
	.version2	resd 1
	.entry		resd 1
	.pheader	resd 1
	.sheader	resd 1
	.flags		resd 1
	.hsize		resw 1
	.pesize		resw 1
	.penum		resw 1
	.sesize		resw 1
	.senum		resw 1
	.names		resw 1
endstruc

struc elfprogramheader
	.stype		resd 1
	.offset		resd 1
	.start_virt	resd 1
	.undef		resd 1
	.size_file	resd 1
	.size_mem	resd 1
	.flags		resd 1
	.padding	resd 1
endstruc

%define ISA_X86 3

; ------------
; SECTION DATA
section .data
; ------------

S_ERR		db "ELF ERROR", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Load ELF file
; Loads elf file STATICALLY (thus no linking or repositioning)
;	eax:	Address of elf file
;	edx:	Length of elf file
;	->eax:	NULL if succeeded, else entry address
; -------------
global elf_load
elf_load:
		push	ebp
		mov		ebp, esp
		sub		esp, 12		; -4:	Address of elf file
							; -8:	Lenght of elf file
							; -12:	ebx
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ebx

		; Check if ELF is compatible
		mov		edx, eax
		cmp		dword [edx+elfheader.magic], 0x464c457f
		jnz		.error				; Data is not ELF
		cmp		byte [edx+elfheader.mode], 1
		jnz		.error				; ELF is not 32b
		cmp		byte [edx+elfheader.endianness], 1
		jnz		.error				; ELF is not little endian
		cmp		word [edx+elfheader.type], 2
		jnz		.error				; ELF is not executable
		cmp		word [edx+elfheader.ISA], ISA_X86
		jnz		.error				; ELF is not x86

		; Traverse all header entries
		; Get program header
		xor		ecx, ecx
		xor		ebx, ebx
		mov		eax, [edx+elfheader.pheader]
		mov		cx, [edx+elfheader.penum]
		mov		bx, [edx+elfheader.pesize]
		add		edx, eax
.lp:
		; Check segment
		mov		eax, [edx+elfprogramheader.stype]
		cmp		eax, 1
		jne		.next
		; Loadable segment
		push	edx
		push	ecx

		; Allocate memory
		push	edx
		call	proc_getmemory
		pop		edx
		mov		edi, eax
		mov		eax, [edx+elfprogramheader.start_virt]
		mov		edx, [edx+elfprogramheader.size_mem]
		mov		ecx, 0 ; TODO FLAGS
		call	vmm_alloc
		pop		ecx
		pop		edx
		; Copy data
		push	ecx
		mov		ecx, [edx+elfprogramheader.size_file]
		mov		esi, [edx+elfprogramheader.offset]
		add		esi, [ebp-4]
		mov		edi, [edx+elfprogramheader.start_virt]
	rep	movsb
		pop		ecx
		push	ecx
		; If memsize is bigger then file (bss) zero out
		; TODO zero out
		pop		ecx
.next:
		add		edx, ebx
		dec		ecx
		jnz		.lp

		; Return entry address
		mov		edx, [ebp-4]
		mov		eax, [edx+elfheader.entry]
.end:
		mov		ebx, [ebp-12]
		mov		esp, ebp
		pop		ebp
		ret
.error:
		mov		eax, S_ERR
		call	serial_outs
		jmp		.end
