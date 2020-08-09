#include <i386/interrupts.h>

struct interrupt_frame;
extern idt_set_interrupt_c(unsigned int num, void (*handler)());
extern idt_set_interrupt_user_c(unsigned int num, void (*handler)());

void remap_PIC();
void isr_empty_N(struct interrupt_frame * frame);
void isr_empty_E(struct interrupt_frame * frame, unsigned int error_code);

// EXCEPTION HANDLERS
// ------------------
#define ISR_N(name, shortname) __attribute__((interrupt)) void isr_ ## shortname (struct interrupt_frame* frame){ \
        asm volatile("cli"); \
        debug_print_s(name); \
        for(;;); \
    }
#define ISR_E(name, shortname) __attribute__((interrupt)) void isr_ ## shortname (struct interrupt_frame* frame, unsigned int error_code){ \
        debug_print_s(name); \
        debug_print_sd("\r\n error code: ", error_code); \
        for(;;); \
    }

ISR_N("Divide-by-zero", dz)
ISR_N("Debug", db)
ISR_N("Non-maskable Interrupt", nmi)
ISR_N("Breakpoint", br)
ISR_N("Overflow", of)
ISR_N("Bound Range Exceeded", be)
ISR_N("Invalid Opcode", io)
ISR_N("Device Not Available", dn)
ISR_N("Double Fault", df)
ISR_N("Coprocessor Segment Overrun", cs)
ISR_N("Invalid TSS", ts)
ISR_N("Segment Not Present", sn)
ISR_N("Stack-Segment Fault", ss)
ISR_N("General Protection Fault", gp)
ISR_N("Page Fault", pf)
ISR_N("x87 Floating-Point Exception", 87)
ISR_N("Alignment Check", ac)
ISR_N("Machine Check", mc)
ISR_N("SIMD Floating-Point Exceptions", si)
ISR_N("Virtualization Exception", vi)
ISR_N("Security Exception", se)

// ------------------

int interrupts_init(){
    debug_print_s("Initializing interrupts\r\n");

    remap_PIC();
    // Fill PIC interrupts with empty isr
    for(int i=0x20; i<0x30; i++){
        idt_set_interrupt_c(i, isr_empty_N);
    }

    // Setup exception handlers
    idt_set_interrupt_c(0, isr_dz);

    asm volatile("sti");
    asm volatile("int $0");
    return 0;
}

void remap_PIC(){
    io_outb(0x20, 0x11);
    io_outb(0xa0, 0x11);        // Restart PICs
    io_outb(0x21, 0x20);        // PIC1 starts at 0x20
    io_outb(0xa1, 0x28);        // PIC2 starts at 0x28
    io_outb(0x21, 0x04);
    io_outb(0xa1, 0x02);        // Setup cascading
    io_outb(0x21, 0x01);
    io_outb(0xa1, 0x01);        // 8086 mode
    io_outb(0x21, 0x00);
    io_outb(0xa1, 0x00);        // Disable all interrupts
}

__attribute__((interrupt)) void isr_empty_N(struct interrupt_frame * frame){
    io_outb(0x20, 0x20);
}
__attribute__((interrupt)) void isr_empty_E(struct interrupt_frame * frame, unsigned int error_code){
    debug_print_s("PANIC!!!\r\n");
    asm volatile("cli");
    for(;;);
}