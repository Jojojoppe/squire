#include "../../common/arch/debugprint.h"
#include "config.h"
#include "io.h"

#ifdef KERNEL_LOG_PRINT_ENABLE

void arch_debugprint_putc(char c){
    while(!io_inb(0x03f8+5));
    io_outb(0x03f8+0, c);
}

void arch_debugprint_init(){
    io_outb(0x03f8+1, 0x0a);
    io_outb(0x03f8+3, 0x80);
    io_outb(0x03f8+0, 0x03);
    io_outb(0x03f8+2, 0x00);
    io_outb(0x03f8+3, 0x03);
    io_outb(0x03f8+2, 0xc7);
    io_outb(0x03f8+4, 0x0b);
}

#endif
