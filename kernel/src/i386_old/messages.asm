bits 32

; INCLUDES
; --------
%include "proc.inc"
%include "kmalloc.inc"
; --------

struc message
	.next		resd 1		; Next message in line
	.from		resd 1		; Process descriptor from sender
	.size		resd 1		; Data size
	.data:
	.sizeof:
endstruc

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

; Add message to queue
;	eax:	data
;	edx:	from
;	ecx:	size
;	edi:	process descriptor to add to
; --------------------
global message_add
message_add:
		push	ebp
		mov		ebp, esp
		sub		esp, 24		; -4:	data
							; -8:	from
							; -12:	size
							; -16:	process descriptor
							; -20:	message structure
							; -24:	old message structure from descriptor
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx
		mov		[ebp-16], edi

		; Allocate message structure
		mov		eax, [ebp-12]
		add		eax, message.sizeof
		call	kmalloc
		mov		[ebp-20], eax

		; Add message structure to message list
		; Save old message structure address
		mov		eax, [ebp-16]
		call	proc_getmessages
		mov		[ebp-24], eax
		; Set new message structure
		mov		eax, [ebp-16]
		mov		edx, [ebp-20]
		call	proc_setmessages
		; Set next to old one
		mov		eax, [ebp-24]
		mov		edx, [ebp-20]
		mov		[edx+message.next], eax
		; Fill other fieds
		mov		eax, [ebp-8]
		mov		[edx+message.from], eax
		mov		eax, [ebp-12]
		mov		[edx+message.size], eax
		; Copy message
		mov		edi, edx
		add		edi, message.data
		mov		esi, [ebp-4]
		mov		ecx, eax
	rep	movsb

		mov		esp, ebp
		pop		ebp
		ret

; Get message from queue
;	eax:	pointer to data buffer
;	edx:	max size of data buffer
;	ecx:	process descriptor to get from
;	->eax:	length of recieved message, NULL if none
;	->edx:	PID of sender, NULL if none
; ----------------------
global message_get
message_get:
		push	ebp
		mov		ebp, esp
		sub		esp, 12		; -4:	pointer to data buffer
							; -8:	max size of data buffer
							; -12:	process descriptor
		mov		[ebp-4], eax
		mov		[ebp-8], edx
		mov		[ebp-12], ecx

		; Check if there is a message
		mov		eax, ecx
		call	proc_getmessages
		test	eax, eax
		jz		.nomessage

		; Get last message in list
		mov		edx, eax
.next:
		mov		ecx, eax
		mov		eax, edx
		mov		edx, [edx+message.next]
		test	edx, edx
		jnz		.next
		mov		edx, eax
		; ecx: previous, edx: last
		; Unlink last one
		cmp		ecx, edx
		je		.clearall
		mov		dword [ecx+message.next], 0
.process:
		; Get message size
		mov		eax, [edx+message.size]
		mov		ecx, [ebp-8]
		cmp		ecx, eax
		jb		.nospace
		push	eax

		; Copy data to buffer
		mov		edi, [ebp-4]
		lea		esi, [edx+message.data]
		mov		ecx, eax
	rep	movsb

		; Get PID of sender
		mov		eax, [edx+message.from]
		call	proc_getpid
		push	eax
		; Free message structure
		mov		eax, edx
		call	kfree
		pop		edx
		pop		eax
.end:
		mov		esp, ebp
		pop		ebp
		ret
.nomessage:
		xor		eax, eax
		mov		edx, eax
		jmp		.end
.nospace:
		xor		edx, edx
		jmp		.end
.clearall:
		push	edx
		xor		edx, edx
		mov		eax, [ebp-12]
		call	proc_setmessages
		pop		edx
		jmp		.process

