bits 32

%include "serial.inc"

%define MBOOT_magic					0x1badb002
%define MBOOT_flags					0x07
%define MBOOT_checksum				-(MBOOT_magic+MBOOT_flags)
%define KERNEL_virtualbase			0xc0000000

struc mbootinfo
	.flags				resd 1
	.mem_lower			resd 1
	.mem_upper			resd 1
	.boot_device		resd 1
	.cmdline			resd 1
	.mods_count			resd 1
	.mods_addr			resd 1
	.syms				resd 4
	.mmap_length		resd 1
	.mmap_addr			resd 1
	.drives_length		resd 1
	.drives_addr		resd 1
	.config_table		resd 1
	.boot_loader_name	resd 1
	.apm_table			resd 1
	.vbe_control_info	resd 1
	.vbe_mode_info		resd 1
	.vbe_mode			resd 1
	.vbe_interface_seg	resw 1
	.vbe_interface_off	resw 1
	.vbe_interface_len	resw 1
	.framebuffer_addr	resq 1
	.framebuffer_pitch	resd 1
	.framebuffer_width	resd 1
	.framebuffer_height resd 1
	.framebuffer_bpp	resb 1
	.framebuffer_type	resb 1
	.color_info			resb 6
endstruc

struc mmap_entry
	.size				resd 1
	.base_addr			resq 1
	.length				resq 1
	.type				resb 1
endstruc

struc mod_entry
	.start				resd 1
	.end				resd 1
	.string 			resd 1
	.reserved			resd 1
endstruc

; -------------
; SECTION MBOOT
section .mboot
align 0x04
; -------------
									dd MBOOT_magic
									dd MBOOT_flags
									dd MBOOT_checksum
									dd 0
									dd 0
									dd 0
									dd 0
									dd 0
;									dd 1				; EGA text
;									dd 80				; Width
;									dd 25				; Height
;									dd 0				; Depth
									dd 0				; Linear graphics
									dd 640				; Width
									dd 480				; Height
									dd 32				; Depth

; ------------
; SECTION DATA
section .data
; ------------

mboot_base_addr		resd 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Get memory map field
; Get the address and length of a usable memory region
;	eax:	address of previous mmap field, 0 if first
;	edx:	edx should contain address of mboot info header
;	->eax:	address of current mmap field, 0 if last
;	->edx:	base address of memory region
;	->ecx;	length of memory region, 0 if not usable
; --------------------
global mboot_get_mmap
mboot_get_mmap:
		push	ebp
		mov		ebp, esp
		sub		esp, 12			; -4:	mmap_addr
								; -8:	mmap_length
								; -12:	address of previous map
		; Save mboot info header address for later use
		mov		[mboot_base_addr], edx

		test	eax, eax
		jz		.first_entry
		jmp		.next_entry

.proceed:
		mov		[ebp-12], eax
		; load mmap_addr and mmap_length
		mov		eax, [edx + mbootinfo.mmap_addr]
		add		eax, KERNEL_virtualbase
		mov		ecx, eax
		mov		[ebp-4], eax
		mov		eax, [edx + mbootinfo.mmap_length]
		mov		[ebp-8], eax
		add		ecx, eax

		; Check if last
		mov		eax, [ebp-12]
		cmp		eax, ecx
		jl		.getinfo
		; Last entry
		xor		eax, eax
		jmp		.return

.getinfo:
		mov		eax, [ebp-12]
		mov		edx, [eax + mmap_entry.base_addr]
		mov		ecx, [eax + mmap_entry.length]
		; Check type of region
		cmp		byte [eax + mmap_entry.type], 1
		je		.return
		xor		ecx, ecx
.return:
		mov		esp, ebp
		pop		ebp
		ret
.first_entry:
		; Check if mmap is present
		mov		eax, [edx + mbootinfo.flags]
		bt		eax, 6
		jnc		.error
		; Load address of first mmap entry
		mov		eax, [edx + mbootinfo.mmap_addr]
		add		eax, KERNEL_virtualbase
		jmp		.proceed
.error:
		xor		eax, eax
		mov		esp, ebp
		pop		ebp
		ret
.next_entry:
		add		eax, [eax + mmap_entry.size]
		add		eax, 4
		jmp		.proceed

; Get boot module
;	eax:	string of name
;	->eax:	address of module
;	->edx:	length of module
; ---------------
global mboot_get_mod
mboot_get_mod:
		push	ebp
		mov		ebp, esp
		sub		esp, 4			; -4: String of name
		mov		[ebp-4], eax

		; Get mod list
		mov		edx, [mboot_base_addr]
		mov		ecx, [edx+mbootinfo.mods_count]
		mov		edx, [edx+mbootinfo.mods_addr]
		add		edx, KERNEL_virtualbase

.lp:
		push	ecx
		push	edx

		; Get module name/parameter string
		mov		eax, [edx+mod_entry.string]
		add		eax, KERNEL_virtualbase
		mov		edx, [ebp-4]
		call	strcmp
		test	eax, eax
		jnz		.next
		; Module found
		pop		edx
		mov		eax, [edx+mod_entry.end]
		sub		eax, [edx+mod_entry.start]
		push	eax
		mov		eax, [edx+mod_entry.start]
		add		eax, KERNEL_virtualbase
		pop		edx
		mov		esp, ebp
		pop		ebp
		ret

.next:
		pop		edx
		pop		ecx
		; Goto next module
		add		edx, 16
		dec		ecx
		jnz		.lp

		; If here: module not found
		xor		eax, eax
		mov		esp, ebp
		pop		ebp
		ret

; String compare
;	eax, edx:	strings to compare
;	->eax:		NULL if the same
strcmp:
		push	ebp
		mov		ebp, esp
		mov		[ebp-4], ecx
		mov		ecx, eax
.lp:
		mov		al, [ecx]
		cmp		al, [edx]
		jne		.nsame
		; Same character
		test	al, al
		jz		.same
		; Goto next
		inc		ecx
		inc		edx
		jmp		.lp		

.same:
		xor		eax, eax
		mov		ecx, [ebp-4]
		mov		esp, ebp
		pop		ebp
		ret
.nsame:
		mov		eax, 1
		mov		ecx, [ebp-4]
		mov		esp, ebp
		pop		ebp
		ret
