bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "vmm.inc"
%include "kmalloc.inc"
%include "serial.inc"
%include "vas.inc"
; --------

struc process
	.next		resd 1
	.prev		resd 1

	.id			resd 1				; Process ID
	.memory		resd 1				; Memory regions list address
	.vas		resd 1				; CR3 for process
	.threads	resd 1				; Pointer to thread list

	.sizeof:
endstruc

struc thread
	.next		resd 1
	.prev		resd 1

	.id			resd 1
	.kstack		resd 1
	.tss_esp0	resd 1

	.sizeof:
endstruc

; ------------
; SECTION DATA
section .data
; ------------

S_00			db "SWITCH!", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
align 0x04
; -----------

; Task state segment
; ------------------
TSS					resd 26

; PID counter
proc_pidcounter		resd 1
; Current process
proc_proccurrent	resd 1
proc_threadcurrent	resd 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize processes
;	eax:	main kernel thread function
;	->eax:	memory region list
;	->edx:	thread descriptor
; --------------------
global proc_init
proc_init:
		push	ebp
		mov		ebp, esp
		cli
		sub		esp, 4
		mov		[ebp-4], eax

		; Initialize TSS
		mov		eax, TSS
		call	gdt_settss
		mov		edx, TSS
		mov		dword [edx+1*4], 0			; ESP0
		mov		dword [edx+2*4], 0x10		; SS0
		mov		dword [edx+25*4], 104		; IOPB

		; Set PID counter
		mov		eax, 2
		mov		[proc_pidcounter], eax

		; Create main process structure
		mov		eax, process.sizeof
		call	kmalloc
		mov		[proc_proccurrent], eax
		; Fill process structure
		mov		edx, eax
		mov		[edx+process.next], edx
		mov		[edx+process.prev], edx
		mov		dword [edx+process.id], 1
		; Create memory region list
		push	edx
		call	vmm_create
		pop		edx
		mov		dword [edx+process.memory], eax
		mov		eax, cr3
		mov		dword [edx+process.vas], eax
		; Create main thread structure
		mov		eax, thread.sizeof
		push	edx
		call	kmalloc
		pop		edx
		mov		dword [edx+process.threads], eax
		push	eax
		; Fill tread structure
		mov		edx, eax
		mov		dword [edx+thread.next], 0
		mov		dword [edx+thread.prev], 0
		mov		dword [edx+thread.id], 1
		extern boot_stack_top
		mov		dword [edx+thread.kstack], boot_stack_top-44
		mov		dword [edx+thread.tss_esp0], 0

		; Create return stack frame
		mov		edx, boot_stack_top
		mov		eax, [ebp-4]
		mov		[edx-4], eax			; Return address
		mov		eax, 0
		mov		[edx-8], eax			; Old ebp
		mov		[edx-12], eax			; eax
		mov		[edx-16], eax			; ecx
		mov		[edx-20], eax			; edx
		mov		[edx-24], eax			; ebx
		mov		eax, boot_stack_top-8
		mov		[edx-28], eax			; esp (=ebp)
		mov		[edx-32], eax			; ebp
		mov		eax, 0
		mov		[edx-36], eax			; esi
		mov		[edx-40], eax			; edi
		; Get eflags
		pushfd
		pop		eax
		mov		[edx-44], eax			; eflags

		mov		edx, [proc_proccurrent]
		mov		eax, [edx+process.memory]
		pop		edx
		mov		esp, ebp
		pop		ebp
		ret

; Switch to thread
;	eax:	address of thread structure to switch to
;	edx:	address of thead structure to save thread information to (NULL if not saving)
; ----------------
global proc_thread_switch
proc_thread_switch:
		push	ebp
		mov		ebp, esp
		cli

		mov		[proc_threadcurrent], eax

		; Check if need to save
		test	edx, edx
		jz		.endsave

		; OLD STACK
		pushad
		pushfd
		mov		[edx+thread.kstack], esp
		mov		ecx, [TSS+4]
		mov		[edx+thread.tss_esp0], ecx
.endsave:
		mov		esp, [eax+thread.kstack]
		mov		ecx, [eax+thread.tss_esp0]
		mov		[TSS+4], ecx
		; NEW STACK
		popfd
		popad

		mov		esp, ebp
		pop		ebp
		ret

; Return current process memory list
;	->eax:	address of memory list
; ----------------------------------
global proc_getmemory
proc_getmemory:
		push	ebp
		mov		ebp, esp
		cli

		mov		edx, [proc_proccurrent]
		mov		eax, [edx+process.memory]

		mov		esp, ebp
		pop		ebp
		ret

; Execute in process/thread
; Sets up user stack and kernel stack and jumps to ring 3
;	eax:	Address to jump to
;	edx:	User stack address
; -------------------------
global proc_user_exec
proc_user_exec:
		push	ebp
		mov		ebp, esp
		cli
		sub		esp, 8		; -4:	Address to jump to
							; -8:	User stack address
		mov		[ebp-4], eax
		mov		[ebp-8], edx

		; Load current esp as kernel stack in TSS
		mov		edx, TSS
		mov		eax, esp
		mov		dword [edx+1*4], esp		; ESP0
		mov		dword [edx+2*4], 0x10		; SS0

		; Jump to user space
		cli
		mov		eax, 0x23					; User data segment
		mov		ds, ax
		mov		es, ax
		mov		fs, ax
		mov		gs, ax

		push	eax							; User stack segment
		mov		eax, [ebp-8]
		push	eax							; User stack
		pushfd
		pop		eax
		or		eax, 0x200
		push	eax							; EFLAGS with interrupts re-enabled
		mov		eax, 0x1b
		push	eax							; User code segment
		mov		eax, [ebp-4]
		push	eax							; New user code

		; Clear registers
		xor		eax, eax
		mov		ebx, eax
		mov		ecx, eax
		mov		edx, eax
		mov		edi, eax
		mov		esi, eax
		mov		ebp, [ebp-8]

		iret

; Start a thread
; This is done to re-enable interrupts
; DO NOT CALL DIRECTLY! This is put on a new kernel thread stack
; --------------
_proc_thread_start:
		int 1
		sti
		ret

; New kernel thread
;	eax:	Code to execute
;	edx:	Stack
;	ecx:	Process to add thread to
;	->eac:	Thread structure address
; -----------------
global proc_thread_new
proc_thread_new:
		push	ebp
		mov		ebp, esp
		cli
		sub		esp, 20			; -4:	Code to execute
								; -8:	Stack
								; -12:	Process to add thread to
								; -16:	thread descriptor
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		cli

		; Allocate new thread structure
		mov		eax, thread.sizeof
		call	kmalloc
		mov		[ebp-16], eax
		; Link thread structure to said process
		mov		edx, [ebp-12]
		; Find last in line
		mov		eax, [edx+process.threads]
.findlast:
		mov		edx, eax
		mov		eax, [edx+thread.next]
		test	eax, eax
		jnz		.findlast
		; edx contains last thread
		; Link
		mov		ecx, [ebp-16]
		mov		[edx+thread.next], ecx
		mov		[ecx+thread.prev], edx
		mov		dword [ecx+thread.next], 0
		; Fill other data
		mov		eax, [edx+thread.id]
		inc		eax
		mov		[ecx+thread.id], eax
		mov		eax, [ebp-8]
		sub		eax, 48
		mov		[ecx+thread.kstack], eax
		mov		dword [ecx+thread.tss_esp0], 0

		; Create return stack frame
		mov		edx, [ebp-8]
		mov		eax, [ebp-4]
		mov		[edx-4], eax			; Return address
		mov		eax, _proc_thread_start
		mov		[edx-8], eax			; Return address of _proc_thread_start
		mov		eax, 0
		mov		[edx-12], eax			; Old ebp
		mov		[edx-16], eax			; eax
		mov		[edx-20], eax			; ecx
		mov		[edx-24], eax			; edx
		mov		[edx-28], eax			; ebx
		mov		eax, [ebp-8]
		sub		eax, 12
		mov		[edx-32], eax			; esp (=ebp)
		mov		[edx-36], eax			; ebp
		mov		eax, 0
		mov		[edx-40], eax			; esi
		mov		[edx-44], eax			; edi
		; Get eflags
		pushfd
		pop		eax
		mov		[edx-48], eax			; eflags

		mov		eax, [ebp-16]
		sti
		mov		esp, ebp
		pop		ebp
		ret

; New user thread
;	eax:	Code to execute
;	edx:	Stack
;	ecx:	Process to add thread to
;	->eac:	Thread structure address
; -----------------
global proc_thread_new_user
proc_thread_new_user:
		push	ebp
		mov		ebp, esp
		sub		esp, 20			; -4:	Code to execute
								; -8:	(user) Stack
								; -12:	Process to add thread to
								; -16:	Kernel stack
								; -20:	Thread structure
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		cli

		; TODO IMPLEMENT
		; Create kernel stack
		; Setup kernel stack frame containing the proc_user_exec function

		; Create kernel stack
		mov		eax, 1			; 4KiB of kernel stack
		call	vas_kbrk_addx
		add		eax, 4096-4
		mov		[ebp-16], eax

		; Allocate new thread structure
		mov		eax, thread.sizeof
		call	kmalloc
		mov		[ebp-20], eax
		; Link thread structure to said process
		mov		edx, [ebp-12]
		; Find last in line
		mov		eax, [edx+process.threads]
.findlast:
		mov		edx, eax
		mov		eax, [edx+thread.next]
		test	eax, eax
		jnz		.findlast
		; edx contains last thread
		; Link
		mov		ecx, [ebp-20]
		mov		[edx+thread.next], ecx
		mov		[ecx+thread.prev], edx
		mov		dword [ecx+thread.next], 0
		; Fill other data
		mov		eax, [edx+thread.id]
		inc		eax
		mov		[ecx+thread.id], eax
		mov		eax, [ebp-16]
		sub		eax, 48							; TODO HOW MUCH??
		mov		[ecx+thread.kstack], eax
		mov		dword [ecx+thread.tss_esp0], 0

		; Create return stack frame
		mov		edx, [ebp-16]
		mov		eax, proc_user_exec		; Last return address
		mov		[edx-4], eax
		mov		eax, _proc_thread_start
		mov		[edx-8], eax			; Return address of thread start
		mov		eax, 0
		mov		[edx-12], eax			; Old ebp
		mov		eax, [ebp-4]
		mov		[edx-16], eax			; eax
		mov		eax, 0
		mov		[edx-20], eax			; ecx
		mov		eax, [ebp-8]
		mov		[edx-24], eax			; edx
		mov		eax, 0
		mov		[edx-28], eax			; ebx
		mov		eax, [ebp-16]
		sub		eax, 12
		mov		[edx-32], eax			; esp (=ebp)
		mov		[edx-36], eax			; ebp
		mov		eax, 0
		mov		[edx-40], eax			; esi
		mov		[edx-44], eax			; edi
		; Get eflags
		pushfd
		pop		eax
		mov		[edx-48], eax			; eflags

		mov		eax, [ebp-20]
		sti
		mov		esp, ebp
		pop		ebp
		ret

; Return current process
;	->eax:	current process
; ----------------------------------
global proc_getcurrent
proc_getcurrent:
		push	ebp
		mov		ebp, esp
		cli

		mov		eax, [proc_proccurrent]

		mov		esp, ebp
		pop		ebp
		ret

; Scheduler
; Called by the timer interrupt or after syscall
; ---------
global proc_schedule
proc_schedule:
		push	ebp
		mov		ebp, esp
		cli

		; Check if there is next thread
		mov		edx, [proc_threadcurrent]
		mov		eax, [edx+thread.next]
		test	eax, eax
		jz		.nextproc
		call	proc_thread_switch
		jmp		.end

.nextproc:
		mov		edx, [proc_proccurrent]
		nop
		nop									; Add stability... Probably cache miss??
		mov		edx, [edx+process.next]
		mov		[proc_proccurrent], edx
		mov		eax, [edx+process.threads]
		mov		edx, [proc_threadcurrent]
		call	proc_thread_switch

.end:
		mov		esp, ebp
		pop		ebp
		ret
