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

// -----------
// SECTION BSS
.section .bss
.align 0x04
// -----------

// Boot time stacks
// ----------------
.global stack_IRQ_0_t
.global stack_IRQ_0_b
stack_IRQ_0_t:
.skip 0x1000
stack_IRQ_0_b:
.global stack_FIQ_0_t
.global stack_FIQ_0_b
stack_FIQ_0_t:
.skip 0x1000
stack_FIQ_0_b:
.global stack_ABT_0_t
.global stack_ABT_0_b
stack_ABT_0_t:
.skip 0x1000
stack_ABT_0_b:
.global stack_UND_0_t
.global stack_UND_0_b
stack_UND_0_t:
.skip 0x1000
stack_UND_0_b:
.global stack_SYS_0_t
.global stack_SYS_0_b
stack_SYS_0_t:
.skip 0x1000
stack_SYS_0_b:
.global stack_SVC_0_t
.global stack_SVC_0_b
stack_SVC_0_t:
.skip 0x1000
stack_SVC_0_b:

// ------------
// SECTION VTAB
.section .vtab
.align 0x04
// ------------

// Main CPU0 interrupt vector table
// --------------------------------
.global cpu0_vtable
cpu0_vtable:
		b		_start			// SVC: Reset
		b		.				// UND: Undefined
		b		.				// SVC: SVC instruction
		b		.				// ABT: Invalid instruction address
		b		.				// ABT: R/W to invalid address
		b		.				// HYP: Hypervisor
		b		.				// IRQ: IRQ interrupt
		b		.				// FIQ: FIQ interrupt

// ------------
// SECTION TEXT
.section .text
.align 0x04
// ------------

// ENTRY
// -----
.globl _start
_start:
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
		mcr		p15, #0, r0, c12, c0, #0

		.extern archmain
		bl		archmain

		// Hang
		b		.
