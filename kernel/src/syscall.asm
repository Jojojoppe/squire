bits 32

; INCLUDES
; --------
%include "serial.inc"
%include "proc.inc"
%include "vmm.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------

S_00			db "SYSCALL", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Syscall numbers
%define SYSCALL_YIELD		0x00000000
%define SYSCALL_MMAP		0x00000001
%define SYSCALL_THREAD		0x00000010
%define SYSCALL_DEBUG		0xffffffff

; Syscall ISR
; -----------
global isr_syscall
isr_syscall:
		cli
		push	ebp
		mov		ebp, esp
		pushad
		sub		esp, 12			; -36:	return value
								; -40:	Syscall opcode
								; -44:	Syscall parameter memory block
								; -48:	Syscall parameter memory block length
		mov		[ebp-40], eax
		mov		[ebp-44], edx
		mov		[ebp-48], ecx

		; Clear out return value
		xor		eax, eax
		mov		[ebp-36], eax
		mov		eax, [ebp-40]

		cmp		eax, SYSCALL_YIELD
		je		syscall_yield
		cmp		eax, SYSCALL_MMAP
		je		syscall_mmap
		cmp		eax, SYSCALL_THREAD
		je		syscall_thread
		cmp		eax, SYSCALL_DEBUG
		je		syscall_debug

.error:
		; No syscall found
		mov		eax, -1
		mov		[ebp-36], eax

.end:
		; Set return value in eax on stack (pushfd)
		mov		eax, [ebp-36]
		mov		[ebp-4], eax
		; Return to program
		add		esp, 12
		popad
		mov		esp, ebp
		pop		ebp
		sti
		iret

; YIELD
; Triggers scheduler
;	-> NULL
syscall_yield:
		call	proc_schedule
		mov		dword [ebp-36], 0
		jmp		isr_syscall.end

; MMAP
; Map a page in the process space
;	-> NULL if successful
;	.address will point to allocated address
;	.length will have the allocated length
;	.flags will have the flags of allocated memory
struc params_mmap
	.address	resd 1		; Starting address of range to map. NULL if kernel may choose		[0, 0x00400000 - 0xbfffffff]
	.length		resd 1		; Length of range to map in bytes. Kernel will floor to 4KiB		[0x1000 - 0x10000000]
	.flags		resd 1		; Flags for mapping (TODO implement and define flags)
	.sizeof:
endstruc
syscall_mmap:
		; Check for block length
		cmp		ecx, params_mmap.sizeof
		jb		isr_syscall.error

		; Check if length of region is OK
		mov		eax, [edx+params_mmap.length]
		cmp		eax, 0x1000
		jb		isr_syscall.error
		cmp		eax, 0x10000000
		ja		isr_syscall.error
		; Check if base address is within OK region
		mov		eax, [edx+params_mmap.address]
		cmp		eax, 0xc0000000
		jnb		isr_syscall.error
		test	eax, eax
		jz		.kernel_select
		cmp		eax, 0x00400000
		jb		isr_syscall.error
		; User chooses memory region

		push	edx
		call	proc_getmemory
		mov		edi, eax
		pop		edx
		mov		eax, [edx+params_mmap.address]
		mov		ecx, [edx+params_mmap.flags]
		mov		edx, [edx+params_mmap.length]
		and		edx, ~0xfff
		call	vmm_alloc
		test	eax, eax
		jnz		isr_syscall.error

		mov		dword [ebp-36], 0
		jmp		isr_syscall.end
.kernel_select
		; Let kernel choose memory region
		jmp		isr_syscall.error
		; TODO not yet implemented
		mov		dword [ebp-36], 0
		jmp		isr_syscall.end

; THREAD
; Create a new thread
;	->NULL if succeeded
;	.entry will contain the TID of the new thread
;	.stack will contain the PID of the current process
struc params_thread
	.entry		resd 1		; Entry of the code to run					[0x00400000 - 0xbfffffff]
	.stack		resd 1		; Points towards the top of the stack		[0x00400000 - 0xbfffffff]
	.flags		resd 1		; Flags for new thread (TODO implement and define flags)
	.sizeof:
endstruc
syscall_thread:
		; Check for block length
		cmp		ecx, params_thread.sizeof
		jb		isr_syscall.error

		; Check if entry is OK
		mov		eax, [edx+params_thread.entry]
		cmp		eax, 0x00400000
		jb		isr_syscall.error
		cmp		eax, 0xc0000000
		ja		isr_syscall.error
		; Check if stack is OK
		mov		eax, [edx+params_thread.stack]
		cmp		eax, 0x00400000
		jb		isr_syscall.error
		cmp		eax, 0xc0000000
		ja		isr_syscall.error

		call	proc_getcurrent
		mov		ecx, eax
		mov		eax, [edx+params_thread.entry]
		mov		edx, [edx+params_thread.stack]
		call	proc_thread_new_user

		mov		dword [ebp-36], 0
		jmp		isr_syscall.end

; DEBUG
; Triggers int 1 (kernel panic debug
;	-> NULL if successful
syscall_debug:
		int		1
		mov		dword [ebp-36], 0
		jmp		isr_syscall.end

; Debug syscall ISR
; -----------
global isr_syscall_debug
isr_syscall_debug:
		pushad
		cli

		; Check if address is valid
		cmp		eax, 0xc0000000
		jnb		.end
		cmp		eax, 0x00400000
		jnae	.end
		call	serial_outs

.end:
		call	proc_schedule
		popad
		sti
		iret
