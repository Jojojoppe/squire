#include "cpu.h"

void cpu_enable_interrupts(){
	asm("sti");
}

void cpu_disable_interrupts(){
	asm("cli");
}
