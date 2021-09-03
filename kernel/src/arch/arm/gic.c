#include "gic.h"
#include "cpu.h"
#include "../../common/arch/vas.h"

static gic_distributor_registers * gic_dregs;
static gic_cpu_interface_registers * gic_ifregs;

void gic_init(){

    gic_dregs = (gic_distributor_registers*)(cpu_get_periphbase() + CPU_ICD_BASE);
    gic_ifregs = (gic_cpu_interface_registers*)(cpu_get_periphbase() + CPU_ICC_BASE);

    // Map GIC registers to VAS
    arch_vas_map(gic_dregs, gic_dregs, VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE);
    arch_vas_map(gic_ifregs, gic_ifregs, VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE);


    gic_dregs->DCTLR = 0;                   // Disable interrupt disruptor
    gic_ifregs->CCTLR = 3|8;                // Enable interrupt forwarding @ FIQ
    gic_ifregs->CCPMR = 0xff;               // Enable all interrupt priorities
    gic_dregs->DCTLR = 1;                   // Enable interrupt disruptor
}

void gic_enable_interrupt(unsigned char number, unsigned char priority){
    unsigned char reg = number/32;
    unsigned char bit = number%32;

    gic_dregs->DCTLR = 0;                   // Disable interrupt disruptor
    gic_dregs->DISENABLER[reg] |= (1u<<bit);

    // Set priority
    reg = number/4;
    bit = (number%4)*8;
    gic_dregs->DIPRIORITY[reg] &= ~(0xff<<bit);
    gic_dregs->DIPRIORITY[reg] |= (number&0xf8)<<bit;

    // Forward to CPU 0
    reg = number/4;
    bit = (number%4)*8;
    gic_dregs->DITARGETSR[reg] &= ~(0x3<<bit);
    gic_dregs->DITARGETSR[reg] |= (1u<<bit);
    gic_dregs->DCTLR = 1;                   // Enable interrupt disruptor
}

unsigned int gic_ack_interrupt(){
    return gic_ifregs->CIAR & 0x3ff;
}

void gic_end_interrupt(unsigned char number){
    gic_ifregs->CEOIR = number & 0x3ff;
}

void gic_sgi(unsigned char target, unsigned char cpu, unsigned char irq){
    unsigned int * dsgir = (unsigned int*)(cpu_get_periphbase() + 0x1f00);
    *dsgir = target<<24 | cpu<<16 | irq&0xf;
    *dsgir = target<<24 | cpu<<16;
}