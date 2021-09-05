#ifndef __H_IDT
#define __H_IDT 1

typedef struct{
	unsigned int ip;
	unsigned int cs;
	unsigned int fs;
	unsigned int sp;
	unsigned int ss;
}__attribute__((packed)) isr_frame;

void idt_init();

void idt_set_interrupt_gate(unsigned char entry, void * address, unsigned char dpl);

void idt_set_trap_gate(unsigned char entry, void * address, unsigned char dpl);

#endif
