#include "../../config.h"

// -----------
// SECTION BSS
.section .bss
.align 0x1000
// -----------

// Boot time stack
// ---------------
.global stack_b
.global stack_t
stack_b:
.skip 0x1000
stack_t:

// Boot time heap
// --------------
.global boot_heap_start
boot_heap_start:
.skip BOOT_HEAP_LENGTH*0x1000

// ------------
// SECTION DATA
.section .data
.align 0x1000
// ------------

// Boot time page directory
// ------------------------
// Identity maps first 4MB to 0x00000000 and 0xc0000000
.global boot_pd
boot_pd:
.long 0x00000083
.fill 767, 4, 0x00000000
.long 0x00000083
.fill 255, 4, 0x00000000

// Global descriptor table
// -----------------------
.global GDT
GDT:
    # NULL
    .long       0
    .long       0

    # KERNEL CODE
	.short		0xffff		# limit 0:15
	.short		0x0000		# base 0:15
	.byte		0x00		# base 16:23
	.byte		0x9a		# access byte (10011010) {PR, PRIV[2], S, Ex, DC, RW, AC}
	.byte		0xcf		# flags (1100) {GR, Sz} - limit 16:19
	.byte		0x0000		# base 24:31

    # KERNEL DATA
	.short		0xffff		# limit 0:15
	.short		0x0000		# base 0:15
	.byte		0x00		# base 16:23
	.byte		0x92		# access byte (10010010) {PR, PRIV[2], S, Ex, DC, RW, AC}
	.byte		0xcf		# flags (1100) {GR, Sz} - limit 16:19
	.byte		0x0000		# base 24:31

    # USER CODE
	.short		0xffff		# limit 0:15
	.short		0x0000		# base 0:15
	.byte		0x00		# base 16:23
	.byte		0xfa		# access byte (11111010) {PR, PRIV[2], S, Ex, DC, RW, AC}
	.byte		0xcf		# flags (1100) {GR, Sz} - limit 16:19
	.byte		0x0000		# base 24:31

    # USER DATA
	.short		0xffff		# limit 0:15
	.short		0x0000		# base 0:15
	.byte		0x00		# base 16:23
	.byte		0xf2		# access byte (11110010) {PR, PRIV[2], S, Ex, DC, RW, AC}
	.byte		0xcf		# flags (1100) {GR, Sz} - limit 16:19
	.byte		0x0000		# base 24:31

    # TSS
    .short      0x0067
    .short      0x0000
    .byte       0x0
    .byte       0x89
    .byte       0x40
    .byte       0x0
GDT_END:
.global GDTR
GDTR:
    .short      GDT_END-GDT
    .long       GDT

// ------------
// SECTION TEXT
.section .mboot
.align 0x04
// ------------

// MBOOT
// -----
.set MBOOT_MAGIC,       0x1badb002
.set MBOOT_FLAGS,       0x7
//.set MBOOT_FLAGS,       0x3
.set MBOOT_CHECKSUM,    -(MBOOT_MAGIC+MBOOT_FLAGS)

.long MBOOT_MAGIC
.long MBOOT_FLAGS
.long MBOOT_CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
.long 1
.long 80
.long 25
.long 0

// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

// ENTRY
// -----
.global _start
_start:
		// Check if loaded by multiboot compliant loader
		cli
		cmp		$0x2badb002, %eax
		jne		hang

		// Setup paging
		mov		$boot_pd-0xc0000000, %eax
		mov		%eax, %cr3					// Set page table address to boot page table
		mov		%cr4, %eax
		or		$0x00000010, %eax			// Enable Page size extension
		mov		%eax, %cr4
		mov		%cr0, %eax
		or		$0x80000001, %eax			// Enable paging and 
		mov		%eax, %cr0
		lea 	(_start.high), %eax
		jmp 	*%eax
_start.high:
		// Disable first 4Mb identity mapping
		movl	$0, (boot_pd)
		// Setup recursive mapping of page table itself
		mov		$boot_pd-0xc0000000, %eax
		and		$0xfffff000, %eax
		or 		$0x00000003, %eax
		mov		%eax, (boot_pd+4092)
		invlpg	(0)

		// Load stack
		mov		$stack_t, %esp
		mov		%esp, %ebp

		// Install GDT
		mov		$GDTR, %eax
		lgdt	(%eax)
		mov		$0x10, %ax
		mov		%ax, %ds
		mov		%ax, %es
		mov		%ax, %fs
		mov		%ax, %gs
		mov		%ax, %ss
		ljmp	$0x08, $_start.loadcs
_start.loadcs:

		// Goto C environment
		mov		$boot_heap_start, %eax
		push	%eax
		add 	$0xc0000000, %ebx
		push	%ebx
		.extern archmain
		call	archmain

hang:
		cli
		hlt
		jmp		hang
