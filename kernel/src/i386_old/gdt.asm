bits 32

; ------------
; SECTION DATA
section .data
align 0x04
; ------------

; GDT
; ---
; 64 bits each, 4 normal, 1 null and 1 tss make 48 byte
global gdt_base
gdt_base:
.null:
		dq		0
.kernel_code:
		dw		0xffff			; Base 0:15
		dw		0x0000			; Limit 0:15
		db		0				; Base 24:31
		db		0x9a			; Flags | limit 16:19
		db		0xcf			; Acces byte
		db		0				; Base 16:23
.kernel_data:
		dw		0xffff			; Base 0:15
		dw		0x0000			; Limit 0:15
		db		0				; Base 24:31
		db		0x92			; Flags | limit 16:19
		db		0xcf			; Acces byte
		db		0				; Base 16:23
.user_code:
		dw		0xffff			; Base 0:15
		dw		0x0000			; Limit 0:15
		db		0				; Base 24:31
		db		0xfa			; Flags | limit 16:19		RING 3
		;db		0x9a			; Flags | limit 16:19		RING 0
		db		0xcf			; Acces byte
		db		0				; Base 16:23
.user_data:
		dw		0xffff			; Base 0:15
		dw		0x0000			; Limit 0:15
		db		0				; Base 24:31
		db		0xf2			; Flags | limit 16:19		RING 3
		;db		0x92			; Flags | limit 16:19		RING 0
		db		0xcf			; Acces byte
		db		0				; Base 16:23
.tss:
		dw		0x0067			; Base 0:15
		dw		0x0000			; Limit 0:15
		db		0				; Base 24:31
		db		0x89			; Flags | limit 16:19
		db		0x40			; Acces byte
		db		0				; Base 16:23
gdt_end:

global gdtr
gdtr:
		dw		gdt_end-gdt_base	; Size
		dd		gdt_base		; Offset

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Install gdt
; -----------
global gdt_install
gdt_install:
		push	ebp
		mov		ebp, esp

		mov		eax, gdtr
		lgdt	[eax]				; Load table pointer
		mov		ax, 0x10
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax
		mov		ss, ax				; Load all data segments
		jmp		0x08:.loadcs		; Jump to load code segment
.loadcs:

		mov		esp, ebp
		pop		ebp
		ret

; Set TSS
;	eax:	address of TSS
; -------
global gdt_settss
gdt_settss:
		push	ebp
		mov		ebp, esp

		mov		edx, gdt_base
		mov		[edx+42], ax
		shr		eax, 16
		mov		[edx+44], al
		shr		eax, 8
		mov		[edx+47], al
		mov		ax, 0x2b
		ltr		ax

		mov		esp, ebp
		pop		ebp
		ret
