bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "interrupts.inc"
%include "serial.inc"
; --------

%define KERNEL_virtualbase		0xc0000000
%define KERNEL_pagenum			768
%define KERNEL_stacksize		0x4000
%define KERNEL_PT				0xffc00000
%define KERNEL_PD				0xfffff000

; ------------
; SECTION DATA
section .data
align 0x1000
; ------------

; Boot time page directory
; ------------------------
boot_PD:
								dd 0x00000083			; First 4MB at 0x00000000
times (KERNEL_pagenum-1)		dd 0
								dd 0x00000083			; First 4MB at 0xc0000000
times (1024-KERNEL_pagenum-1)	dd 0

; Strings
; -------
S_00							db 0x0a, 0x0d, "JOS - A simple OS written in assembly", 0x0a, 0x0d, 0x00

; -----------
; SECTION BSS
section .bss
align 0x1000
; -----------

; Boot time kernel stack
; ----------------------
boot_stack_btm					resb KERNEL_stacksize
boot_stack_top:

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Entry point
; -----------
global g_start
g_start:
		cli
		; Check if loaded by mboot compliant loader
		; If not, hang
		cmp		eax, 0x2badb002
		jne		hang

		; Load page directory table and enable paging
		mov		eax, boot_PD - KERNEL_virtualbase
		mov		cr3, eax				; Load boot time page directory
		mov		eax, cr4
		or		eax, 0x00000010
		mov		cr4, eax				; Enable 4MB paging
		mov		eax, cr0
		or		eax, 0x80000010
		mov		cr0, eax				; Enable paging and read-only on user+kernel ring
		; Move to higher half
		lea		eax, [.start_high]
		jmp		.start_high
.start_high:
		; Disable first 4MB at 0x00000000
		mov		dword [boot_PD], 0
		; Recursive map page directory
		mov		eax, boot_PD - KERNEL_virtualbase
		and		eax, 0xfffff000
		or		eax, 0x00000003
		mov		[boot_PD+4*1023], eax
		; invlpg [0] ??

		; Setup temporary kernel stack
		mov		esp, boot_stack_top
		mov		ebp, esp

		; Load GDT
		call	gdt_install

		; Initialize interrupts
		call	interrupts_init

		; Initialize serial port
		call	serial_init
		; Write kernel name to tty
		mov		eax, S_00
		call	serial_outs

		mov		eax, 5
		mov		ebx, 0
		div		ebx

hang:
		cli
		hlt
		jmp		hang
