bits 32

%define MBOOT_magic					0x1badb002
%define MBOOT_flags					0x07
%define MBOOT_checksum				-(MBOOT_magic+MBOOT_flags)

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
