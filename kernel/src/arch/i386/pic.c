#include "pic.h"
#include "io.h"
#include "idt.h"

__attribute__((interrupt)) void pic_empty_isr(isr_frame * frame){
	pic_ack();
}

void pic_init(){
	unsigned char a1 = io_inb(0x21);
    unsigned char a2 = io_inb(0xa1);
    io_outb(0x20, 0x11);
    io_outb(0xa0, 0x11);        // Restart PICs
    io_outb(0x21, 0x20);        // PIC1 starts at 0x20
    io_outb(0xa1, 0x28);        // PIC2 starts at 0x28
    io_outb(0x21, 0x04);
    io_outb(0xa1, 0x02);        // Setup cascading
    io_outb(0x21, 0x01);
    io_outb(0xa1, 0x01);        // 8086 mode
    io_outb(0x21, a1);
    io_outb(0xa1, a2);          // Restore masks

	// Install empty ISR's
	for(unsigned char i=0x20; i<0x30; i++){
		idt_set_interrupt_gate(i, pic_empty_isr, 3);
	}
}

void pic_ack(){
	io_outb(0xa0, 0x20);
    io_outb(0x20, 0x20);
}
