#include "cpu.h"

unsigned int cpu_get_periphbase(){    
    unsigned int res;
    asm("mrc p15, #4, %0, c15, c0, #0" : "=r"(res));
    return res;
}

void cpu_enable_interrupts(){
    asm("cpsie if");
}

void cpu_disable_interrupts(){
    asm("cpsid if");        
}