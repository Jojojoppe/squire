#include "cpu.h"

unsigned int cpu_get_periphbase(){    
    unsigned int res;
    asm("mrc p15, #4, %0, c15, c0, #0" : "=r"(res));
    return res;
}

void cpu_enable_interrupts(){
    unsigned int cpsr_val;
    asm("mrs %0, cpsr" : "=r"(cpsr_val));
    cpsr_val &= ~(0x80 | 0x40);
    asm("msr cpsr, %0" ::"r"(cpsr_val));
}

void cpu_disable_interrupts(){
    unsigned int cpsr_val;
    asm("mrs %0, cpsr" : "=r"(cpsr_val));
    cpsr_val |= 0x80 | 0x40;
    asm("msr cpsr, %0" ::"r"(cpsr_val));
}
