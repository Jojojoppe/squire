bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "interrupts.inc"
%include "serial.inc"
%include "mboot.inc"
%include "pmm.inc"
%include "vas.inc"
%include "kmalloc.inc"
%include "proc.inc"
%include "timer.inc"
%include "vmm.inc"
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
S_00							db 0x0a, 0x0d,
								db "+---------------+", 0x0a, 0x0d
								db "|    SQUIRE     |", 0x0a, 0x0d
								db "+---------------|", 0x0a, 0x0d
								db "| a microkernel |", 0x0a, 0x0d
								db "+---------------+", 0x0a, 0x0d, 0
S_RN							db 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
align 0x1000
; -----------

; Boot time kernel stack
; ----------------------
boot_stack_btm					resb KERNEL_stacksize
global boot_stack_top
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

		; Load GDT
		call	gdt_install

		; Initialize interrupts
		call	interrupts_init

		; Initialize serial port
		call	serial_init
		; Write kernel name to tty
		mov		eax, S_00
		call	serial_outs

		; Initialize pmm
		mov		eax, [ebp-4]
		call	pmm_init
		; Initialize kernel VAS
		; kernel break is set at 4MiB. Heap starts after that
		call	vas_init

		; Initialize kmalloc
		call	kmalloc_init

		; Initialize processing
		sub		esp, 64			; Make space on stack
		; Proc_init uses top as kernel stack for start of new kernel thread stack
		; Since upcomming function calls must be able to return without overwriting set-up stack frame
		; Space must be made
		mov		eax, .after_proc_init
		call	proc_init
		push	eax
		push	edx

		; Initialize timer
		call	timer_init

		; Jump to kernel thread
		xor		edx, edx
		pop		eax
		call	proc_thread_switch
.after_proc_init:
		; From here stack frame is reset! From now on running in process 1, thread 1 ([1,1])
		push	ebp
		mov		ebp, esp

		; Create test thread
		; Create kernel stack
		mov		eax, 4
		call	vas_kbrk_addx
		add		eax, 4096*4
		mov		[edx], eax
		mov		eax, .lp
		; Add thread to list
		push	eax
		push	edx
		call	proc_getcurrent
		mov		ecx, eax
		pop		edx
		pop		eax
		call	proc_thread_new

		; For test copy usertest thread to userspace
		call	proc_getmemory
		mov		edi, eax
		mov		eax, 0x00400000
		mov		edx, 0x4000
		mov		ecx, 0
		call	vmm_alloc
		; Copy usertest to userspace (dummy elf load)
		mov		ecx, usertest_end-usertest
		mov		esi, usertest
		mov		edi, 0x00400000
	rep	movsb
		; Setup user stack
		call	proc_getmemory
		mov		edi, eax
		mov		eax, 0x2fffb000
		mov		edx, 0x4000
		mov		ecx, 0
		call	vmm_alloc
		; Execute code in userspace
		mov		eax, 0x00400000
		mov		edx, 0x2fffb000
		call	proc_user_exec

.lp:
		call	timer_print
		mov		al, 0x0d
		call	serial_out
		jmp		.lp

hang:
		cli
		hlt
		jmp		hang

; USER TEST THREAD
; ----------------
usertest:
		nop
		jmp		0x1b:0x00400000
usertest_end:
