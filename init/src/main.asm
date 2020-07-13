bits 32

; Entry point
; -----------
global _start
_start:
		push	ebp
		mov		ebp, esp

.lp:
		nop
		jmp		.lp
