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
		b		.				// 04 UND: Undefined
		b		.				// 08 SVC: SVC instruction
		b 		ab_rwinvalid	// 0c ABT: Invalid instruction address
		b		ab_rwinvalid	// 10 ABT: R/W to invalid address
		b		.				// 14 HYP: Hypervisor
		b		irq				// 18 IRQ: IRQ interrupt
		b		.				// 1c FIQ: FIQ interrupt

// ------------
// SECTION DATA
.section .data
.align 0x04

.global boot_pd_c
boot_pd_c:
.long boot_pd
// ------------

// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

ab_rwinvalid:
		mov r0, r14
		.extern arch_rwinvalid
		bl		arch_rwinvalid
		b 		.

irq:
		.extern arch_irq
		bl 		arch_irq

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

		// Identity map first 4 MB to 0x00000000 and 0xc0000000
		ldr		r0, =boot_pd-KERNEL_BASE
		ldr		r2, =boot_pd-KERNEL_BASE
		add		r2, r2, #12288
		ldr		r1, =0x00000402
		str		r1, [r0, #0]			// pt[000] = 00000402
		str		r1, [r2, #0]			// pt[c00] = 00000402
		ldr		r1, =0x00100402
		str		r1, [r0, #4]			// pt[001] = 00100402
		str		r1, [r2, #4]			// pt[c01] = 00100402
		ldr		r1, =0x00200402
		str		r1, [r0, #8]			// pt[002] = 00200402
		str		r1, [r2, #8]			// pt[c02] = 00200402
		ldr		r1, =0x00300402
		str		r1, [r0, #12]			// pt[003] = 00300402
		str		r1, [r2, #12]			// pt[c03] = 00300402

		// Setup paging on arm
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
		str		r1, [r0, #0]			// pt[000] = 0
		str		r1, [r0, #4]			// pt[001] = 0
		str		r1, [r0, #8]			// pt[002] = 0
		str		r1, [r0, #12]			// pt[003] = 0

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
