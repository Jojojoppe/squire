// -----------
// SECTION BSS
.section .bss
.align 0x04
// -----------

// Boot time stack
// ---------------
.global stack_t
.global stack_b
stack_t:
.skip 0x1000
stack_b:

// ------------
// SECTION TEXT
.section .mboot
.align 0x04
// ------------

// MBOOT
// -----
.set MBOOT_MAGIC,       0x1badb002
//.set MBOOT_FLAGS,       0x7 with VBE
.set MBOOT_FLAGS,       0x3
.set MBOOT_CHECKSUM,    -(MBOOT_MAGIC+MBOOT_FLAGS)

.long MBOOT_MAGIC
.long MBOOT_FLAGS
.long MBOOT_CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
//.long 1
//.long 80
//.long 25
//.long 0

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

		// Load stack
		mov		$stack_t, %esp
		mov		%esp, %ebp

		.extern archmain
		call	archmain

hang:
		cli
		hlt
		jmp		hang
