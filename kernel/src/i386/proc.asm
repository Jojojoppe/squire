bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "vmm.inc"
%include "kmalloc.inc"
%include "serial.inc"
%include "vas.inc"
%include "pmm.inc"
%include "elf.inc"
; --------

struc process
	.next		resd 1
	.prev		resd 1

	.id			resd 1				; Process ID
	.memory		resd 1				; Memory regions list address
	.vas		resd 1				; CR3 for process
	.threads	resd 1				; Pointer to thread list
	.messages	resd 1				; Pointer to message structure. NULL if none has been set up

	.sizeof:
endstruc

struc thread
	.next		resd 1
	.prev		resd 1

	.id			resd 1
	.kstack		resd 1
	.tss_esp0	resd 1
	.fpudata	resb 128

	.sizeof:
endstruc

; ------------
; SECTION DATA
section .data
; ------------

S_00			db "SWITCH!", 0x0a, 0x0d, 0
S_RN			db 0x0a, 0x0d, 0

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
		mov		dword [edx+process.messages], 0
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

		; Save FPU state
		fsave	[edx+thread.fpudata]

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
		
		; Restore FPU data
		frstor	[eax+thread.fpudata]

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
;	ecx:	argv data
;		word size, data
; 	ebx:	argc
; -------------------------
global proc_user_exec
proc_user_exec:
		push	ebp
		mov		ebp, esp
		cli
		sub		esp, 16		; -4:	Address to jump to
							; -8:	User stack address
							; -12:	Param data
							; -16:	Param count
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		mov		[ebp-16], ebx

		; Add params to user stack
		mov		edi, [ebp-8]
		; Put all data on stack
		mov		ecx, [ebp-16]
		mov		edx, [ebp-12]
.lp:
		; Get size and push
		mov		eax, [edx]
		push	eax
		; Copy data
		add		edx, 4
		sub		edi, eax
		push	edi
.lp_cp:
		mov		bl, [edx]
		inc 	edx
		mov		[edi], bl
		inc		edi
		dec 	eax
		jnz		.lp_cp
		; Done copying
		; Set size to user stack
		pop		edi
		pop		eax
		sub		edi, 4
		mov		[edi], eax
		dec		ecx
		jnz		.lp
		mov		ecx, [ebp-16]
		sub		edi, 4
		mov		[edi], ecx				; Put argc on stack
		; Save new stack pointer
		mov		[ebp-8], edi

		; Load current esp as kernel stack in TSS
		mov		edx, TSS
		mov		eax, esp
		mov		dword [edx+1*4], esp		; ESP0
		mov		dword [edx+2*4], 0x10		; SS0

		; Jump to user space
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
		mov		eax, [ebp-12]
		mov		ebp, [ebp-8]

		iret

; Start a thread
; This is done to re-enable interrupts
; DO NOT CALL DIRECTLY! This is put on a new kernel thread stack
; --------------
_proc_thread_start:
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
;	ecx:	Process to add thread to, NULL to add to process directly
;	ebx:	argc
;	edi:	param data
;	->eac:	Thread structure address
; -----------------
global proc_thread_new_user
proc_thread_new_user:
		push	ebp
		mov		ebp, esp
		sub		esp, 28			; -4:	Code to execute
								; -8:	(user) Stack
								; -12:	Process to add thread to
								; -16:	Kernel stack
								; -20:	Thread structure
								; -24:	argc
								; -28:	param data
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		mov		[ebp-24], ebx
		mov		[ebp-28], edi
		cli

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
		test	edx, edx
		jz		.nolink
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
.fillother:
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
		mov		eax, [ebp-28]
		mov		[edx-20], eax			; ecx
		mov		eax, [ebp-8]
		mov		[edx-24], eax			; edx
		mov		eax, [ebp-24]
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
		mov		esp, ebp
		pop		ebp
		ret
.nolink:
		mov		edx, [proc_proccurrent]
		mov		[edx+process.threads], eax
		mov		dword [eax+thread.id], 1
		mov		ecx, eax
		jmp		.fillother

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
		mov		eax, [edx+process.vas]
		mov		cr3, eax
		mov		eax, [edx+process.threads]
		mov		edx, [proc_threadcurrent]
		call	proc_thread_switch

.end:
		mov		esp, ebp
		pop		ebp
		ret

; Create new process
;	eax:	Start of ELF data
;	edx:	argc
;	ecx:	param data
;	->eax:	Process descriptor
; ------------------
global proc_process_new
proc_process_new:
		push	ebp
		mov		ebp, esp
		cli
		sub		esp, 20		; -4:	Start of ELF data
							; -8:	Process descriptor
							; -12:	temporary proc_proccurrent
							; -16:	argc
							; -20:	param data
		mov		[ebp-4], eax
		mov		[ebp-16], edx
		mov		[ebp-20], ecx

		; Create new process descriptor
		mov		eax, process.sizeof
		call	kmalloc
		mov		[ebp-8], eax
		mov		ecx, eax
		; Link descriptor in list
		mov		edx, [proc_proccurrent]
		mov		[ecx+process.prev], edx
		mov		eax, [edx+process.next]
		mov		[ecx+process.next], eax
		mov		[edx+process.next], ecx
		mov		edx, [ecx+process.next]
		mov		[edx+process.prev], ecx
		; Set PID
		mov		eax, [proc_pidcounter]
		mov		[ecx+process.id], eax
		inc		dword [proc_pidcounter]
		; Create memory region list
		call	vmm_create
		mov		ecx, [ebp-8]
		mov		[ecx+process.memory], eax
		; Clear message structure
		mov		dword [ecx+process.messages], 0
		; Create new PD table and copy kernel space into it (use 0 as tmp)
		call	pmm_alloc
		push	eax
		mov		ecx, [ebp-8]
		mov		[ecx+process.vas], eax
		xor		edx, edx
		call	vas_map
		; Copy kernel PD
		mov		esi, 0xfffff000 + 768*4
		mov		edi, 768*4
		mov		ecx, 255*4		; Not the last one (that must be cr3 itself)
	rep	movsb
		pop		eax
		or		eax, 3
		mov		[edi], eax
		push	eax
		; Clear first part of kernel PD
		mov		edi, 0
		mov		ecx, 256*4*3
		xor		eax, eax
	rep	stosb
		pop		eax

		; SWITCH TO NEW PROCESS VAS
		mov		eax, [proc_proccurrent]
		mov		[ebp-12], eax
		mov		edx, [ebp-8]
		mov		[proc_proccurrent], edx
		mov		eax, [edx+process.vas]
		mov		cr3, eax

		; Load elf
		mov		eax, [ebp-4]
		call	elf_load
		push	eax
		; TODO test if succeeded
		; Setup user stack
		call	proc_getmemory
		mov		edi, eax
		mov		eax, 0xbfffc000
		mov		edx, 0x4000
		mov		ecx, 0
		call	vmm_alloc
		pop		eax


		; Add thread descriptor with [eax] as entry point
		xor		ecx, ecx
		mov		edx, 0xc0000000-4
		mov		ebx, [ebp-16]
		mov		edi, [ebp-20]
		call	proc_thread_new_user

		; SWITCH BACK TO OWN PROCESS VAS
		mov		edx, [ebp-12]
		mov		[proc_proccurrent], edx
		mov		eax, [edx+process.vas]
		mov		cr3, eax

		mov		eax, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret

; Get PID from descriptor
;	eax:	descriptor
;	->eax:	PID
; -----------------------
global proc_getpid
proc_getpid:
		push	ebp
		mov		ebp, esp

		mov		eax, [eax+process.id]

		mov		esp, ebp
		pop		ebp
		ret

; Get Process descriptor from PID
;	eax:	PID
;	->eax:	descriptor, NULL if not found
; -------------------------------
global proc_getprocess
proc_getprocess:
		push	ebp
		mov		ebp, esp

		mov		edx, [proc_proccurrent]
.lp:
		cmp		eax, [edx+process.id]
		je		.found
		; Next process
		mov		edx, [edx+process.next]
		cmp		edx, [proc_proccurrent]
		jne		.lp

		; Not found
		xor		eax, eax
.end:
		mov		esp, ebp
		pop		ebp
		ret
.found:
		mov		eax, edx
		jmp		.end

; Get messages address from descriptor
;	eax:	descriptor
;	->eax:	messages
; -----------------------
global proc_getmessages
proc_getmessages:
		push	ebp
		mov		ebp, esp

		mov		eax, [eax+process.messages]

		mov		esp, ebp
		pop		ebp
		ret

; Set messages address int descriptor
;	eax:	descriptor
;	edx:	messages
; -----------------------
global proc_setmessages
proc_setmessages:
		push	ebp
		mov		ebp, esp

		mov		[eax+process.messages], edx

		mov		esp, ebp
		pop		ebp
		ret
