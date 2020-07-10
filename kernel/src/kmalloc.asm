bits 32

; INCLUDES
; --------
%include "vas.inc"
%include "serial.inc"
; --------

struc heap_block
	.next		resd 1
	.biggest	resd 1
	.size		resd 1
	.padding	resd 1
	.data		resb 1 ;--
endstruc

struc heap_chunk
	.length		resd 1
	.flags		resd 1
	.data		resb 1 ;--
endstruc

%define FLAGS_USED 0
%define FLAGS_LAST 1

; ------------
; SECTION DATA
section .data
; ------------

; -----------
; SECTION BSS
section .bss
; -----------
HEAP_START			resd 1
HEAP_SIZE			resd 1

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

; Initialize kmalloc
; ------------------
global kmalloc_init
kmalloc_init:
		push	ebp
		mov		ebp, esp

		; Create start of heap
		mov		eax, 4			; Allocate 16k of mem
		call	vas_kbrk_addx
		mov		[HEAP_START], eax
		mov		dword [HEAP_SIZE], 0x4000

		; Create first block header
		mov		edi, [HEAP_START]
		mov		dword [edi + heap_block.next], 0
		mov		dword [edi + heap_block.biggest], 0x4000 - 16 - 8
		mov		dword [edi + heap_block.size], 0x4000

		; Create first chunk header
		add		edi, heap_block.data
		mov		dword [edi + heap_chunk.length], 0x4000 - 16 - 8
		mov		dword [edi + heap_chunk.flags], 1<<FLAGS_LAST

		mov		esp, ebp
		pop		ebp
		ret

; kmalloc
;	eax:	number of bytes
;	->eax:	start address
; -------
global kmalloc
kmalloc:
		push	ebp
		mov		ebp, esp
		sub		esp, 32		; -4:	number of bytes
							; -8:	edi
							; -12:	esi
		mov		[ebp-4], eax
		mov		[ebp-8], edi
		mov		[ebp-12], esi
		
		mov		edi, [HEAP_START]
.traverseBlock:
		; Get free size in block
		mov		eax, [edi + heap_block.biggest]
		cmp		eax, [ebp-4]
		jl		.nextblock
		; Block usable
		; Find first usable cunk
		mov		esi, edi
		add		esi, heap_block.data
.traverseChunk:
		; Get free size in chunk
		mov		eax, [esi + heap_chunk.length]
		cmp		eax, [ebp-4]
		jl		.nextchunk
		; Chunk right size
		; Check if free
		mov		ecx, [esi + heap_chunk.flags]
		bts		ecx, FLAGS_USED
		jc		.nextchunk
		mov		[rsi + heap_chunk.flags], ecx
		; Chunk usable
		; Check if there is enough space for another chunk
		sub		eax, [ebp-4]
		cmp		eax, 16
		jl		.finish
		; Ceate new chunk
		; TODO finish 


		mov		edi, [ebp-8]
		mov		esi, [ebp-12]
		mov		esp, ebp
		pop		ebp
		ret
