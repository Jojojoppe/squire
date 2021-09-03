#ifndef __H_CPU
#define __H_CPU 1

unsigned int cpu_get_periphbase();

void cpu_enable_interrupts();
void cpu_disable_interrupts();

// Bases for timers, Watchdogs, Interrupt controller and SCU registers
#define CPU_SCU_BASE    0x0000
#define CPU_ICC_BASE    0x0100
#define CPU_GT_BASE     0x0200
#define CPU_COMP_BASE   0x0210
#define CPU_PT_BASE     0x0600
#define CPU_WDT_BASE    0x0620
#define CPU_ICD_BASE    0x1000

#endif