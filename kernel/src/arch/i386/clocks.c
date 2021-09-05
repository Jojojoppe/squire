#include "../../common/arch/clocks.h"
#include "../../common/kprintf.h"
#include "idt.h"
#include "pic.h"

__attribute__((interrupt)) void pit_isr(isr_frame * frame){
	sysclock_cb();
	pic_ack();
}

void arch_sysclock_init(){
    unsigned short timer_val = 1193182/100;
    io_outb(0x43, 0x36);
    io_outb(0x40, timer_val);
    io_outb(0x40, timer_val >> 8);
}

void arch_sysclock_enable(){
	idt_set_interrupt_gate(0x20, pit_isr, 3);
}

void arch_sysclock_disable(){
	idt_set_interrupt_gate(0x20, pic_empty_isr, 3);
}
