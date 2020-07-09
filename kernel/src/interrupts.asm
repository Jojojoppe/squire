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
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Debug
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Non-maskable Interrupt
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Breakpoint
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Overflow
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Bound Range Exceeded
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Invalid Opcode
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Device Not Available
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Double Fault
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; Coprocessor Segment Overrun
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Invalid TSS
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; Segment not present
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; Stack-Segment Fault
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; General Protection Fault
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; Page Fault
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; x87 Floating-Point Exception
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Alignment Check
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; Machine Check
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; SIMD Floating-Point Exception
		mov		ecx, _isr_panic_no
		call	idt_set_interrupt
		inc		eax						; Virtualization Exception
		mov		ecx, _isr_panic_no
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
		mov		ecx, _isr_panic
		call	idt_set_interrupt
		inc		eax						; RESERVED
		mov		ecx, _isr_empty
		call	idt_set_interrupt
		inc		eax						; Tripple Fault
		mov		ecx, _isr_panic_no
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
_isr_panic_no:
		push	eax					; Placeholder for errorcode
		pushad
		xor		eax, eax
		mov		[esp+32], eax		; Error code
		call	panic
		iret

; Panic ISR code
; --------------
_isr_panic:
		pushad
		call	panic
		sub		esp, 4
		iret
