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
							; -16:	ebx
							; -20:	start new block
							; -24:	size new block
		mov		[ebp-4], eax
		mov		[ebp-8], edi
		mov		[ebp-12], esi
		mov		[ebp-16], ebx

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
		bt		ecx, FLAGS_USED
		jc		.nextchunk
		mov		[esi + heap_chunk.flags], ecx
		; Chunk usable
		; Check if there is enough space for another chunk
		mov		eax, [esi + heap_chunk.length]
		sub		eax, [ebp-4]
		cmp		eax, 16
		jl		.finish
		; Ceate new chunk
		mov		edx, esi
		add		edx, 8
		add		edx, [ebp-4]
		; edx = start address of new chunk
		mov		ebx, [esi + heap_chunk.flags]
		and		ebx, ~(1<<FLAGS_USED)
		mov		[edx + heap_chunk.flags], ebx
		sub		eax, 8
		mov		[edx + heap_chunk.length], eax
		; Remove last flag
		mov		ecx, [esi + heap_chunk.flags]
		btc		ecx, FLAGS_LAST
		mov		[esi + heap_chunk.flags], ecx
		; Set size of chunk
		mov		eax, [ebp-4]
		mov		[esi + heap_chunk.length], eax
.finish:
		; Set used flag
		xor		eax, eax
		or		eax, 1<<FLAGS_USED
		mov		[esi + heap_chunk.flags], eax
		; TODO change biggest if needed
		add		esi, heap_chunk.data
		mov		eax, esi
		jmp		.end
.nextchunk:
		; Check if last chunk
		mov		ecx, [esi + heap_chunk.flags]
		bt		ecx, FLAGS_LAST
		jc		.nextblock
		; There is a next chunk
		mov		eax, [esi + heap_chunk.length]
		add		eax, 8
		add		esi, eax
		jmp		.traverseChunk
.nextblock:
		; Check if last block
		cmp		dword [edi + heap_block.next], 0
		je		.newblock
		; There is next block
		mov		edi, [edi + heap_block.next]
		jmp		.traverseBlock
.newblock:
		mov		eax, [ebp-4]
		add		eax, 16+8
		mov		edx, eax
		and		edx, 0x3fff
		mov		ebx, 0x4000
		sub		ebx, edx		; Remainder on 16k boundary
		add		eax, ebx		; Amount of memory to allocate
		; Allocate memory
		mov		[ebp-24], eax
		shr		eax, 12
		push	edi
		call	vas_kbrk_addx
		mov		[ebp-20], eax
		pop		ebx
		; Memory allocated, starting at -20, size in -24, old block in ebx
		mov		eax, [ebp-20]
		mov		[ebx + heap_block.next], eax

		mov		edi, eax
		mov		dword [edi + heap_block.next], 0
		mov		eax, [ebp-24]
		mov		dword [edi + heap_block.size], eax
		sub		eax, 16+8
		mov		dword [edi + heap_block.biggest], eax

		; Create first chunk header in block
		add		edi, heap_block.data
		mov		dword [edi + heap_chunk.length], eax
		mov		dword [edi + heap_chunk.flags], 1<<FLAGS_LAST

		mov		edi, [ebp-20]
		jmp		.traverseBlock

.end:
		mov		edi, [ebp-8]
		mov		esi, [ebp-12]
		mov		esp, ebp
		pop		ebp
		ret

; kfree
;	eax:	address to free
; -----
global kfree
kfree:
		push	ebp
		mov		ebp, esp
		sub		esp, 8			; -4:	esi
								; -8:	edi
		mov		[ebp-4], esi
		mov		[ebp-8], edi
		mov		edi, eax

		; Get chunk header from memory
		sub		edi, 8
		; Set chunk as free
		mov		eax, [edi + heap_chunk.flags]
		and		eax, ~(1<<FLAGS_USED)
		mov		[edi + heap_chunk.flags], eax

		; Traverse structure to find possible merges
		mov		edi, [HEAP_START]
.traverseBlock:	
		mov		esi, heap_block.data
		add		esi, edi
		xor		edx, edx
.traverseChunk:
		mov		eax, [esi + heap_chunk.flags]
		bt		eax, FLAGS_USED
		jc		.nextchunk
		; Chunk unused
		; Check if there was a previous chunk
		test	edx, edx
		jz		.noprev
		; There was a previous one
		; MERGE!!
		; Add length of chunk to previous one
		mov		eax, [esi + heap_chunk.length]
		add		eax, 8
		add		[edx + heap_chunk.length], eax
		; If current is last one, set last one of previous
		; Done by copying flags
		mov		eax, [esi + heap_chunk.flags]
		mov		[edx + heap_chunk.flags], eax
		; Check if new free size is bigger than stated in block header
		mov		eax, [edi + heap_block.biggest]
		cmp		[edx + heap_chunk.length], eax
		jl		.nobiggest
		; Edit biggest
		mov		eax, [edx + heap_chunk.length]
		mov		[edi + heap_block.biggest], eax
.nobiggest:
		mov		esi, edx
		jmp		.nextchunk
.noprev:
		; No previous chunk, set current one as one
		mov		edx, esi
.nextchunk:
		; Check if last chunk
		mov		eax, [esi + heap_chunk.flags]
		bt		eax, FLAGS_LAST
		jc		.nextblock		
		; Goto next chunk
		mov		eax, [esi + heap_chunk.length]
		add		eax, 8
		add		esi, eax
		jmp		.traverseChunk
.nextblock:
		; Check if last one
		cmp		dword [edi + heap_block.next], 0
		je		.end
		; There is next block
		mov		edi, [edi + heap_block.next]
		jmp		.traverseBlock
.end:
		mov		esi, [ebp-4]
		mov		edi, [ebp-8]
		mov		esp, ebp
		pop		ebp
		ret
