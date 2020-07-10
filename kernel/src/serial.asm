bits 32

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

; Initialize serial port
; ----------------------
global serial_init
serial_init:
		push	ebp
		mov		ebp, esp

		mov		dx, 0x03f8+1
		mov		al, 0x0a
		out		dx, al
		mov		dx, 0x03f8+3
		mov		al, 0x80
		out		dx, al
		mov		dx, 0x03f8+0
		mov		al, 0x03
		out		dx, al
		mov		dx, 0x03f8+2
		xor		al, al
		out		dx, al
		mov		dx, 0x03f8+3
		mov		al, 0x03
		out		dx, al
		mov		dx, 0x03f8+2
		mov		al, 0xc7
		out		dx, al
		mov		dx, 0x03f8+4
		mov		al, 0x0b
		out		dx, al

		mov		esp, ebp
		pop		ebp
		ret

; Out character to serial
;	al:		character to send
; -----------------------
global serial_out
serial_out:
		push	ebp
		mov		ebp, esp
		sub		esp, 4
		mov		[ebp-4], al

.wait:
		mov		dx, 0x03f8+5
		in		al, dx
		test	al, al
		jz		.wait
		mov		dx, 0x03f8+0
		mov		al, [ebp-4]
		out		dx, al

		mov		esp, ebp
		pop		ebp
		ret

; Out a string to serial
;	eax:	address of NULL terminated string to send
; ----------------------
global serial_outs
serial_outs:
		push	ebp
		mov		ebp, esp
		sub		esp, 4
		mov		[ebp-4], esi

		mov		esi, eax
.lp:
		mov		al, [esi]
		test	al, al
		jz		.end
		call	serial_out
		inc		esi
		jmp		.lp

.end:
		mov		esi, [ebp-4]
		mov		esp, ebp
		pop		ebp
		ret

; Out a hex number to serial
;	eax:	number to print
; -------------------------
global serial_outhex
serial_outhex:
		push	ebp
		mov		ebp, esp

		mov		ecx, 8
		rol		eax, 4
.lp:
		push	eax
		push	ecx
		mov		edx, .chars
		and		eax, 0x0000000f
		add		edx, eax
		mov		al, [edx]
		call	serial_out
		pop		ecx
		pop		eax
		rol		eax, 4
		dec		ecx
		jnz		.lp

		mov		esp, ebp
		pop		ebp
		ret
.chars			db "0123456789ABCDEF"

; Out a dec number to serial
;	eax:	number to print
; -------------------------
global serial_outdec
serial_outdec:
		push	ebp
		mov		ebp, esp
		sub		esp, 4+36		; -4:	esi
								; -8:	ebx
								; -12:	destination string
		mov		[ebp-4], esi
		mov		[ebp-8], ebx

		mov		dword [ebp-12], 0
		lea		esi, [ebp-12]
		mov		ebx, 10			; Base 10
.convert:
		xor		edx, edx
		div		ebx
		add		edx, '0'
		cmp		edx, '9'
		jbe		.store
		add		edx, 'A'-'0'-10
.store:
		dec		esi
		mov		[esi], dl
		and		eax, eax
		jnz		.convert

		mov		eax, esi
		call	serial_outs

		mov		esp, ebp
		pop		ebp
		ret

; Out a number to serial with base
;	eax:	number to print
;	edx:	base
; --------------------------------
global serial_outbase
serial_outbase:
		push	ebp
		mov		ebp, esp
		sub		esp, 4+36		; -4:	esi
								; -8:	ebx
								; -12:	destination string
		mov		[ebp-4], esi
		mov		[ebp-8], ebx

		mov		dword [ebp-12], 0
		lea		esi, [ebp-12]
		mov		ebx, edx
.convert:
		xor		edx, edx
		div		ebx
		add		edx, '0'
		cmp		edx, '9'
		jbe		.store
		add		edx, 'A'-'0'-10
.store:
		dec		esi
		mov		[esi], dl
		and		eax, eax
		jnz		.convert

		mov		eax, esi
		call	serial_outs

		mov		esp, ebp
		pop		ebp
		ret
