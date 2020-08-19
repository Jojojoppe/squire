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
