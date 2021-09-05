#include "idt.h"

typedef struct{
	unsigned short offset_l;
	unsigned short selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short offset_h;
}__attribute__((packed)) idt_entry;

typedef struct{
	unsigned short size;
	unsigned int offset;
}__attribute__((packed)) idt_idtr;

idt_entry idt_table[256];

void idt_init(){
	// Fill IDTR
	extern idt_idtr IDTR;
	IDTR.size = sizeof(idt_entry)*256 - 1;
	IDTR.offset = (unsigned int) idt_table;
	// Set IDT in cpu
	asm("lidt %0"::"m"(IDTR));
}

void idt_set_interrupt_gate(unsigned char entry, void * address, unsigned char dpl){
	idt_table[entry].offset_l = (unsigned short)((unsigned int)address&0xffff);
	idt_table[entry].selector = 8;
	idt_table[entry].zero = 0;
	idt_table[entry].type_attr = 0x8e | (dpl&0x3)<<5;
	idt_table[entry].offset_h = (unsigned short)((unsigned int)address>>16);
}

void idt_set_trap_gate(unsigned char entry, void * address, unsigned char dpl){
	idt_table[entry].offset_l = (unsigned short)((unsigned int)address&0xffff);
	idt_table[entry].selector = 8;
	idt_table[entry].zero = 0;
	idt_table[entry].type_attr = 0x8f | (dpl&0x3)<<5;
	idt_table[entry].offset_h = (unsigned short)((unsigned int)address>>16);
}

