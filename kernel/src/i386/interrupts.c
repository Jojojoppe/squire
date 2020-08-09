#include <i386/interrupts.h>

extern idt_set_interrupt_c(unsigned int num, void (*handler)());
extern idt_set_interrupt_user_c(unsigned int num, void (*handler)());

void remap_PIC();
void isr_empty_N();
void isr_empty_E(unsigned int error_code);
extern void isr_test();


// EXCEPTION HANDLERS
// ------------------

struct state{
    unsigned int dr7;
    unsigned int dr6;
    unsigned int dr3;
    unsigned int dr2;
    unsigned int dr1;
    unsigned int dr0;
    unsigned int cr4;
    unsigned int cr3;
    unsigned int cr2;
    unsigned int cr0;
    unsigned int eflags;
    unsigned int gs;
    unsigned int fs;
    unsigned int es;
    unsigned int ds;
    unsigned int ss;
    unsigned int cs;
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;
} __attribute__((packed));

void panic(struct state * s){
    debug_print_s("\r\nEAX: "); debug_print_x(s->eax);
    debug_print_s(" EBX: "); debug_print_x(s->ebx);
    debug_print_s(" ECX: "); debug_print_x(s->ecx);
    debug_print_s(" EDX: "); debug_print_x(s->edx);

    debug_print_s("\r\nESI: "); debug_print_x(s->esi);
    debug_print_s(" EDI: "); debug_print_x(s->edi);
    debug_print_s(" ESP: "); debug_print_x(s->esp);
    debug_print_s(" EBP: "); debug_print_x(s->ebp);

    debug_print_s("\r\nCS: "); debug_print_x(s->cs);
    debug_print_s(" SS: "); debug_print_x(s->ss);
    debug_print_s(" DS: "); debug_print_x(s->ds);
    debug_print_s("\r\nES: "); debug_print_x(s->es);
    debug_print_s(" FS: "); debug_print_x(s->fs);
    debug_print_s(" GS: "); debug_print_x(s->gs);

    debug_print_sx("\r\nEFLAGS: ", s->eflags);

    debug_print_s("CR0: "); debug_print_x(s->cr0);
    debug_print_s(" CR2: "); debug_print_x(s->cr2);
    debug_print_s(" CR3: "); debug_print_x(s->cr3);
    debug_print_s(" CR4: "); debug_print_x(s->cr4);

    debug_print_s("\r\nDR0: "); debug_print_x(s->dr0);
    debug_print_s(" DR1: "); debug_print_x(s->dr1);
    debug_print_s(" DR2: "); debug_print_x(s->dr2);
    debug_print_s(" DR3: "); debug_print_x(s->dr3);
    debug_print_s("\r\nDR6: "); debug_print_x(s->dr6);
    debug_print_s(" DR7: "); debug_print_x(s->dr7);
}

#define ISR_N(name, shortname) void isr_c_ ## shortname (struct state * s){ \
        debug_print_s("\r\nEXCEPTION: "); \
        debug_print_s(name); \
        debug_print_s("\r\n"); \
        panic(s); \
        for(;;); \
    }\
    extern void isr_ ## shortname ();
#define ISR_E(name, shortname) void isr_c_ ## shortname (struct state * s, unsigned int error_code){ \
        debug_print_s("\r\nEXCEPTION: "); \
        debug_print_s(name); \
        debug_print_sx("\r\nerror code: ", error_code); \
        panic(s); \
        for(;;); \
    }\
    extern void isr_ ## shortname ();

ISR_N("Divide-by-zero", dz)
ISR_N("Debug", db)
ISR_N("Non-maskable Interrupt", nmi)
ISR_N("Breakpoint", br)
ISR_N("Overflow", of)
ISR_N("Bound Range Exceeded", be)
ISR_N("Invalid Opcode", io)
ISR_N("Device Not Available", dn)
ISR_E("Double Fault", df)
ISR_N("Coprocessor Segment Overrun", cs)
ISR_E("Invalid TSS", ts)
ISR_E("Segment Not Present", sn)
ISR_E("Stack-Segment Fault", ss)
ISR_E("General Protection Fault", gp)
ISR_E("Page Fault", pf)
ISR_N("x87 Floating-Point Exception", 87)
ISR_E("Alignment Check", ac)
ISR_N("Machine Check", mc)
ISR_N("SIMD Floating-Point Exceptions", si)
ISR_N("Virtualization Exception", vi)
ISR_E("Security Exception", se)

// ------------------

int interrupts_init(){
    remap_PIC();
    // Fill PIC interrupts with empty isr
    for(int i=0x20; i<0x30; i++){
        idt_set_interrupt_c(i, isr_empty_N);
    }

    // Setup exception handlers
    idt_set_interrupt_c(0, isr_dz);
    idt_set_interrupt_c(1, isr_db);
    idt_set_interrupt_c(2, isr_nmi);
    idt_set_interrupt_c(3, isr_br);
    idt_set_interrupt_c(4, isr_of);
    idt_set_interrupt_c(5, isr_be);
    idt_set_interrupt_c(6, isr_io);
    idt_set_interrupt_c(7, isr_dn);
    idt_set_interrupt_c(8, isr_df);
    idt_set_interrupt_c(9, isr_cs);
    idt_set_interrupt_c(10, isr_ts);
    idt_set_interrupt_c(11, isr_sn);
    idt_set_interrupt_c(12, isr_ss);
    idt_set_interrupt_c(13, isr_gp);
    idt_set_interrupt_c(14, isr_pf);
    idt_set_interrupt_c(16, isr_87);
    idt_set_interrupt_c(17, isr_ac);
    idt_set_interrupt_c(18, isr_mc);
    idt_set_interrupt_c(19, isr_si);
    idt_set_interrupt_c(20, isr_vi);
    idt_set_interrupt_c(30, isr_se);

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

void isr_empty_N(){
    io_outb(0x20, 0x20);
}