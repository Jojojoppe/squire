bits 32

; INCLUDES
; --------
%include "idt.inc"
%include "panic.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------

S_DZ				db "divide-by-zero", 0x0a, 0x0d, 0
S_DB				db "debug", 0x0a, 0x0d, 0
S_NMI				db "NMI", 0x0a, 0x0d, 0
S_BR				db "breakpoint", 0x0a, 0x0d, 0
S_OF				db "overflow", 0x0a, 0x0d, 0
S_BE				db "bound range exceeded", 0x0a, 0x0d, 0
S_IO				db "invalid opcode", 0x0a, 0x0d, 0
S_DN				db "device not available", 0x0a, 0x0d, 0
S_DF				db "double fault", 0x0a, 0x0d
S_CS				db "coprocessor segment overrun", 0x0a, 0x0d, 0
S_TS				db "invalid tss", 0x0a, 0x0d, 0
S_SN				db "segment not present", 0x0a, 0x0d, 0
S_SS				db "stack-segment fault", 0x0a, 0x0d, 0
S_GP				db "general protection fault", 0x0a, 0x0d, 0
S_PF				db "page fault", 0x0a, 0x0d,0
S_87				db "x87 floating-point exception", 0x0a, 0x0d, 0
S_AC				db "alignment check", 0x0a, 0x0d, 0
S_MC				db "machine check", 0x0a, 0x0d, 0
S_SI				db "SIMD floating-point exception", 0x0a, 0x0d, 0
S_VI				db "virtualization exception", 0x0a, 0x0d, 0
S_SE				db "sequrity exception", 0x0a, 0x0d, 0
S_TP				db "tripple fault", 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize interrupts
; ---------------------
global interrupts_init
interrupts_init:
		push	ebp
		mov		ebp, esp

		call	_remap_PIC
		call	idt_install

		; Fill idt with isr's for exceptions
		xor		eax, eax				; Divide-by-zero
		mov		ecx, _isr_panic_dz
		call	idt_set_interrupt
		inc		eax						; Debug
		mov		ecx, _isr_panic_db
		call	idt_set_interrupt
		inc		eax						; Non-maskable Interrupt
		mov		ecx, _isr_panic_nmi
		call	idt_set_interrupt
		inc		eax						; Breakpoint
		mov		ecx, _isr_panic_br
		call	idt_set_interrupt
		inc		eax						; Overflow
		mov		ecx, _isr_panic_of
		call	idt_set_interrupt
		inc		eax						; Bound Range Exceeded
		mov		ecx, _isr_panic_be
		call	idt_set_interrupt
		inc		eax						; Invalid Opcode
		mov		ecx, _isr_panic_io
		call	idt_set_interrupt
		inc		eax						; Device Not Available
		mov		ecx, _isr_panic_dn
		call	idt_set_interrupt
		inc		eax						; Double Fault
		mov		ecx, _isr_panic_df
		call	idt_set_interrupt
		inc		eax						; Coprocessor Segment Overrun
		mov		ecx, _isr_panic_cs
		call	idt_set_interrupt
		inc		eax						; Invalid TSS
		mov		ecx, _isr_panic_ts
		call	idt_set_interrupt
		inc		eax						; Segment not present
		mov		ecx, _isr_panic_sn
		call	idt_set_interrupt
		inc		eax						; Stack-Segment Fault
		mov		ecx, _isr_panic_ss
		call	idt_set_interrupt
		inc		eax						; General Protection Fault
		mov		ecx, _isr_panic_gp
		call	idt_set_interrupt
		inc		eax						; Page Fault
		mov		ecx, _isr_panic_pf
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; x87 Floating-Point Exception
		mov		ecx, _isr_panic_87
		call	idt_set_interrupt
		inc		eax						; Alignment Check
		mov		ecx, _isr_panic_ac
		call	idt_set_interrupt
		inc		eax						; Machine Check
		mov		ecx, _isr_panic_mc
		call	idt_set_interrupt
		inc		eax						; SIMD Floating-Point Exception
		mov		ecx, _isr_panic_si
		call	idt_set_interrupt
		inc		eax						; Virtualization Exception
		mov		ecx, _isr_panic_vi
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; Security Exception
		mov		ecx, _isr_panic_se
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; Tripple Fault
		mov		ecx, _isr_panic_tf
		call	idt_set_interrupt


		; Fill idt with empty isr's for hardware interrupts
		mov		eax, 0x29
		mov		ecx, _isr_empty
.lp_hw:
		push	eax
		push	ecx
		call	idt_set_interrupt
		pop		ecx
		pop		eax
		dec		eax
		cmp		eax, 0x19
		jne		.lp_hw

		; Enable interrupts
		sti

		mov		esp, ebp
		pop		ebp
		ret

; Remap PIC
; ---------
_remap_PIC:
		push	ebp
		mov		ebp, esp

		; Remap to interrupts 20-30
		mov		al, 0x11
		out		0x20, al		; Restart PIC1
		out		0xa0, al		; Restart PIC2
		mov		al, 0x20
		out		0x21, al		; PIC1 starts at 0x20
		mov		al, 0x28
		out		0xa1, al		; PIC2 starts at 0x28
		mov		al, 0x04
		out		0x21, al		; Setup cascading
		mov		al, 0x02
		out		0xa1, al		; Setup cascading
		mov		al, 0x01
		out		0x21, al		; 8086 mode
		out		0xa1, al		; 8086 mode
		xor		al, al
		out		0x21, al		; Disable all interrupts
		out		0xa1, al		; Disable all interrupts

		mov		esp, ebp
		pop		ebp
		ret

; Empty ISR
; ---------
_isr_empty:
		iret

; Panic ISR no code
; -----------------
_isr_panic_dz:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		eax, 0xff00f0f0
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_DZ
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_db:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_DB
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_nmi:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_NMI
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_br:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_BR
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_of:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_OF
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_be:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_BE
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_io:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_IO
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_dn:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_DN
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_cs:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_CS
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_87:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_87
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_mc:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_MC
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_si:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_SI
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_vi:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_VI
		mov		[esp+32], eax		; String
		call	panic
		iret
_isr_panic_tf:
		push	eax					; Placeholder for errorcode
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		xor		eax, eax
		mov		[esp+44], eax		; Error code
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_TP
		mov		[esp+32], eax		; String
		call	panic
		iret



; Panic ISR code
; --------------
_isr_panic_df:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_DF
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_ts:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_TS
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_sn:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_SN
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_ss:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_SS
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_gp:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_GP
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_pf:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_PF
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_ac:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_AC
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret
_isr_panic_se:
		push	eax					; Placeholder for cr2
		push	eax					; Placeholder for eip
		push	eax					; String
		pushad
		mov		eax, cr2
		mov		[esp+40], eax		; cr2
		mov		eax, [esp+48]
		mov		[esp+36], eax		; eip
		mov		eax, S_SE
		mov		[esp+32], eax
		call	panic
		sub		esp, 4
		iret

