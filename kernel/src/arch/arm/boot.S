#include "../../config.h"

.syntax unified
.arm

.equ CPSR_MODE_USER,		0x10
.equ CPSR_MODE_FIQ,			0x11
.equ CPSR_MODE_IRQ,			0x12
.equ CPSR_MODE_SVC,			0x13
.equ CPSR_MODE_ABORT,		0x17
.equ CPSR_MODE_UNDEFINED,	0x8B
.equ CPSR_MODE_SYSTEM,		0x1F
.equ CPSR_IRQ,				0x80		// disable IRQ interrupts
.equ CPSR_FIQ,				0x40		// disable FIQ interrupts

#define KERNEL_BASE			0xc0000000

// ------------------
// SECTION BSS.BOOTPD
.section .bss.bootpd
.align 0x04
// ------------------

// Boot time page directory
// ------------------------
.global boot_pd
boot_pd:
.skip 4*4096
.global boot_pt
boot_pt:
.skip 4*256

// -----------
// SECTION BSS
.section .bss
.align 0x04
// -----------

// Boot time stacks
// ----------------
.global stack_IRQ_0_t
.global stack_IRQ_0_b
stack_IRQ_0_b:
.skip 0x1000
stack_IRQ_0_t:
.global stack_FIQ_0_t
.global stack_FIQ_0_b
stack_FIQ_0_b:
.skip 0x1000
stack_FIQ_0_t:
.global stack_ABT_0_t
.global stack_ABT_0_b
stack_ABT_0_b:
.skip 0x1000
stack_ABT_0_t:
.global stack_UND_0_t
.global stack_UND_0_b
stack_UND_0_b:
.skip 0x1000
stack_UND_0_t:
.global stack_SYS_0_t
.global stack_SYS_0_b
stack_SYS_0_b:
.skip 0x1000
stack_SYS_0_t:
.global stack_SVC_0_t
.global stack_SVC_0_b
stack_SVC_0_b:
.skip 0x1000
stack_SVC_0_t:

// Boot time heap
// --------------
.global boot_heap_start
boot_heap_start:
.skip BOOT_HEAP_LENGTH*0x1000

// ------------
// SECTION VTAB
.section .vtab
.align 0x04
// ------------

// Main CPU0 interrupt vector table
// --------------------------------
.global cpu0_vtable
cpu0_vtable:
		b		_start			// 00 SVC: Reset
		hlt						// 04 UND: Undefined
		hlt						// 08 SVC: SVC instruction
		hlt						// 0c ABT: Invalid instruction address
		hlt						// 10 ABT: R/W to invalid address
		hlt						// 14 HYP: Hypervisor
		hlt						// 18 IRQ: IRQ interrupt
		hlt						// 1c FIQ: FIQ interrupt

// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

ex_abt_istr:
		b 		.

ex_abt_data:
		b 		.

// ENTRY
// -----
.globl _start
_start:

		// Disable L1 cache
		mov		r0, #0
		mcr		p15, 0, r0, c7, c7, 0	// Invalidate cache
		mcr		p15, 0, r0, c8, c7, 0	// Invalidate tlb
		mrc 	p15, 0, r0, c1, c0, 0
		bic		r0, r0, #0x1000
		bic		r0, r0, #0x0004
		mcr		p15, 0, r0, c1, c0, 0

		// Fill pagetable with identity mapping and higher half
		ldr		r0, =boot_pd-KERNEL_BASE
		ldr 	r1, =0x00100402
		str 	r1, [r0, #4]			// pt[001] = 00100402
		add		r0, r0, #12288
		str		r1, [r0, #4]			// pt[c01] = 00100402
		// TODO place UART mapping in debugprint.c in platform
		ldr		r0, =boot_pd-KERNEL_BASE
		add 	r0, r0, #12288
		ldr 	r1, =0xe0000402
		str 	r1, [r0, #2048]			// pt[e00] = e0000402
		// Domain 0 to Manager
		mov		r0, #3
		mcr 	p15, 0, r0, c3, c0, 0
		// Set page table address
		ldr		r0, =boot_pd-KERNEL_BASE
		mcr		p15, 0, r0, c2, c0, 0
		mcr		p15, 0, r0, c2, c0, 1
		// Enable paging
		mrc 	p15, 0, r0, c1, c0, 0
		orr 	r0, r0, #1
		orr 	r0, r0, #1<<23
		mcr 	p15, 0, r0, c1, c0, 0

		// Jump to higher half
		ldr 	r0, =.highhalf
		mov		pc, r0
.highhalf:
		// Disable lower half identity mapping
		ldr		r0, =boot_pd
		mov		r1, #0
		str		r1, [r0, #4]			// pt[001] = 0

		// Load stack pointers
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_IRQ)
		ldr		sp, =stack_IRQ_0_t
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_FIQ)
		ldr		sp, =stack_FIQ_0_t
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_ABORT)
		ldr		sp, =stack_ABT_0_t
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_UNDEFINED)
		ldr		sp, =stack_UND_0_t
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_SYSTEM)
		ldr		sp, =stack_SYS_0_t
		msr		CPSR, #(CPSR_IRQ | CPSR_FIQ | CPSR_MODE_SVC)
		ldr		sp, =stack_SVC_0_t

		// Load vector table address
		ldr		r0, =cpu0_vtable
		mcr		p15, 0, r0, c12, c0, 0

		ldr		r0, =boot_heap_start
		.extern archmain
		bl		archmain

		// Hang
		b		.
		hlt