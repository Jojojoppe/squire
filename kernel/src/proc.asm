bits 32

; INCLUDES
; --------
%include "gdt.inc"
%include "vmm.inc"
%include "kmalloc.inc"
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

	.sizeof:
endstruc

; ------------
; SECTION DATA
section .data
; ------------

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
		call	kmalloc
		mov		dword [edx+process.threads], eax
		push	eax
		; Fill tread structure
		mov		edx, eax
		mov		dword [edx+thread.next], 0
		mov		dword [edx+thread.prev], 0
		mov		dword [edx+thread.id], 1
		extern boot_stack_top
		mov		dword [edx+thread.kstack], boot_stack_top-44

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

		; Check if need to save
		test	edx, edx
		jz		.endsave

		; OLD STACK
		pushad
		pushfd
		mov		[edx+thread.kstack], esp
.endsave:
		mov		esp, [eax+thread.kstack]
		; NEW STACK
		popfd
		popad

		sti
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
		iret

