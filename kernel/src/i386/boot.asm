bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "idt.inc"
%include "serial.inc"
%include "mboot.inc"
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

fpu_test						dw 0x55aa

; -----------
; SECTION BSS
section .bss
align 0x1000
; -----------

mboot_header_address			resd 1

; Boot time kernel stack
; ----------------------
boot_stack_btm					resb KERNEL_stacksize
global boot_stack_top
boot_stack_top:
global boot_stack_top_C
boot_stack_top_C				resd 1

global tss_btm
tss_btm							resd 26
tss_top:
global TSS
TSS								resd 1

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
		jmp		eax
.start_high:
		; Disable first 4MB at 0x00000000
		mov		dword [boot_PD], 0
		; Recursive map page directory
		mov		eax, boot_PD - KERNEL_virtualbase
		and		eax, 0xfffff000
		or		eax, 0x00000003
		mov		[boot_PD+4*1023], eax
		invlpg	[0]

		; Setup temporary kernel stack
		mov		esp, boot_stack_top
		mov		ebp, esp
		; Save mboot header address (ebx) on stack
		sub		esp, 4
		add		ebx, KERNEL_virtualbase
		mov		[ebp-4], ebx
		mov		[mboot_header_address], ebx

		; Load GDT
		call	gdt_install

		; Initialize serial port
		call	serial_init

		; Initialize pmm
		mov		eax, [ebp-4]
		push	eax
		extern	pmm_init
		call	pmm_init
		add		esp, 4

		; Initialize interrupts
		call	idt_install
		extern	interrupts_init
		call	interrupts_init

		; Initialize VAS
		extern vas_init
		call	vas_init

		; Set TSS
		mov		eax, tss_btm
		mov		[TSS], eax
		call	gdt_settss

		; Set boot stack top variable
		mov		eax, boot_stack_top_C
		mov		dword [eax], boot_stack_top

		; Create space for stack modification
		sub		esp, 64

		; Check support for x87 FPU
		mov		eax, cr0
		and		eax, ~(1<<2 | 1<<3)
		mov		cr0, eax
		fninit
		fnstsw	[fpu_test]
		cmp		word [fpu_test], 0
		jne		hang

		; Jump to general C
		mov		eax, KERNEL_virtualbase
		push	eax
		mov		eax, [mboot_header_address]
		push	eax
		extern squire_init
		call	squire_init

hang:
		cli
		hlt
		jmp		hang