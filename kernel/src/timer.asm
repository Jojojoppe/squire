bits 32

; INCLUDES
; --------
%include "serial.inc"
%include "proc.inc"
; --------

; ------------
; SECTION DATA
section .data
; ------------

; -----------
; SECTION BSS
section .bss
align 0x04
; -----------

; tick counter
timer_counter		resq 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize timer
; Set timer at 100 hz
; ----------------
global timer_init
timer_init:
		push	ebp
		mov		ebp, esp

		mov		eax, 0x36
		out		0x43, al
		mov		eax, 1193180 / 100
		out		0x40, al
		shr		eax, 8
		out		0x40, al

		mov		esp, ebp
		pop		ebp
		ret

; Timer ISR
; ---------
global isr_timer
isr_timer:
		cli
		pushad

		inc		dword [timer_counter]
		jnc		.end
		inc		dword [timer_counter+4]
.end:
		; Clear PIC
		mov		eax, 0x20
		out		0x20, al

		; Check if scheduler must be called
		mov		eax, [timer_counter]
		and		eax, 0x3f
		cmp		eax, 0x3f
		jnz		.clearpic
		call	proc_schedule
.clearpic:
		popad

		sti
		iret

; Print time
; ----------
global timer_print
timer_print:
		push	ebp
		mov		ebp, esp

		mov		eax, [timer_counter]
		call	serial_outdec

		mov		esp, ebp
		pop		ebp
		ret
