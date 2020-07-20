bits 32

struc tarheader
	.fname		resb 100
	.fmode		resb 8
	.oid		resb 8
	.gid		resb 8
	.fsize		resb 12
	.mod		resb 12
	.checksum	resb 8
	.link		resb 1
	.linkname	resb 100
	.padding	resb 255
	.sizeof:
endstruc

; ------------
; SECTION DATA
section .data
; ------------

S_RN		db 0x0a, 0x0d, 0

; -----------
; SECTION BSS
section .bss
; -----------

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Get file from tar
;	eax:	address of tar
;	edx:	filename of tar
;	->eax:	address of file, NULL if not found
;	->edx:	length of file, NULL if not found
; ------------------
global tar_getfile
tar_getfile:
		push	ebp
		mov		ebp, esp
		sub		esp, 16		; -4:	ebx
							; -8:	ecx
							; -12:	Filename
							; -16:	start of tar
		mov		eax, [ebp+12]
		mov		edx, [ebp+8]
		mov		[ebp-4], ebx
		mov		[ebp-8], ecx
		mov		[ebp-12], edx
		mov		[ebp-16], eax

		mov		edx, eax
.lp:
		; Get size
		lea		eax, [edx+tarheader.fsize]
		push	edx
		call	_ostring2number
		pop		edx
		push	eax

		; Compare filename
		push	ecx
		push	edx
		lea		eax, [edx+tarheader.fname]
		mov		edx, [ebp-12]
		call	_strcmp
		pop		edx
		pop		ecx
		test	eax, eax
		jnz		.next

		; right file
		; Get start of file
		mov		eax, edx
		pop		edx
		mov		ecx, [ebp+16]
		mov		[ecx], edx
		add		eax, tarheader.sizeof
		jmp		.end
		
.next:
		pop		eax
		; To next header
		push	eax
		shr		eax, 9
		inc		eax
		shl		eax, 9
		add		edx, eax
		pop		eax
		and		eax, 0xff
		test	eax, eax
		jz		.noadd
		add		edx, 512
.noadd:
		mov		al, [edx+tarheader.fname]
		test	al, al
		jnz		.lp

		; FILE NOT FOUND
		xor		eax, eax
		mov		edx, eax
.end:
		mov		ebx, [ebp-4]
		mov		ecx, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret


; Print tar content
;	eax:	address of tar
; -----------------
global tar_printlist
tar_printlist:
		push	ebp
		mov		ebp, esp
		sub		esp, 8		; -4:	ebx
							; -8:	ecx
		mov		[ebp-4], ebx
		mov		[ebp-8], ecx

		mov		edx, eax
.lp:
		; Print filename
		lea		eax, [edx+tarheader.fname]
		int		0x81
		mov		eax, S_RN
		int		0x81

		; Get size
		lea		eax, [edx+tarheader.fsize]
		push	edx
		call	_ostring2number
		pop		edx

		; To next header
		push	eax
		shr		eax, 9
		inc		eax
		shl		eax, 9
		add		edx, eax
		pop		eax
		and		eax, 0xff
		test	eax, eax
		jz		.noadd
		add		edx, 512
.noadd:
		mov		al, [edx+tarheader.fname]
		test	al, al
		jnz		.lp

		mov		ebx, [ebp-4]
		mov		ecx, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret

; Octal string to number
;	eax:	octal string
;	->eax:	number
; ----------------------
_ostring2number:
		push	ebp
		mov		ebp, esp
		sub		esp, 16		; -4:	octal string
							; -8:	count
							; -12:	size
		mov		[ebp-4], eax

		mov		dword [ebp-8], 1
		mov		dword [ebp-12], 0
		mov		ecx, 11
		mov		edx, eax
		add		edx, 10
.lp:
		push	edx
		xor		eax, eax
		mov		al, [edx]
		sub		eax, '0'
		mul		dword [ebp-8]
		add		dword [ebp-12], eax
		pop		edx
		dec		edx
		shl		dword [ebp-8], 3
		dec		ecx
		jnz		.lp

		mov		eax, [ebp-12]
		mov		esp, ebp
		pop		ebp
		ret

; String compare
;	eax, edx:	strings to compare
;	->eax:		NULL if the same
_strcmp:
		push	ebp
		mov		ebp, esp
		mov		[ebp-4], ecx
		mov		ecx, eax
.lp:
		mov		al, [ecx]
		cmp		al, [edx]
		jne		.nsame
		; Same character
		test	al, al
		jz		.same
		; Goto next
		inc		ecx
		inc		edx
		jmp		.lp		

.same:
		xor		eax, eax
		mov		ecx, [ebp-4]
		mov		esp, ebp
		pop		ebp
		ret
.nsame:
		mov		eax, 1
		mov		ecx, [ebp-4]
		mov		esp, ebp
		pop		ebp
		ret
