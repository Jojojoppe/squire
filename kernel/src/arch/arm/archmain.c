#include "config.h"
#include "../../common/kprintf.h"
#include "pmm.h"
#include "vas.h"
#include "../../common/kmalloc.h"
#include "gic.h"
#include "cpu.h"

void archmain(void * boot_heap_base){
    // Initialize kernel heap
    kmalloc_init(boot_heap_base, BOOT_HEAP_LENGTH*0x1000);
    // Initialize page frame allocator
    arch_pmm_init();
    arch_vas_init();

    // Initialzize kernel debug log
    kdebuglog_init();
    kprintf("\r\nSQUIRE ARM %s\r\n", VERSION);

    // Print memory information
    kprintf("Amount of available memory:    %d Kb\r\n", arch_pmm_get_available()/1024);
    kprintf("Amount of free memory:         %d Kb\r\n", arch_pmm_get_free()/1024);
    kprintf("Amount of used memory:         %d Kb\r\n", arch_pmm_get_used()/1024);

	// Initialize interrupts
    gic_init();
    cpu_enable_interrupts();

	extern void main();
	main();
	for(;;){
	}
}

void __attribute__((interrupt("IRQ"))) arch_irq(){
    unsigned int irq = gic_ack_interrupt();
	switch(irq&0x3ff){
		case 29:
			arch_sysclock_isr();
			break;
		default:
			break;
	}
    gic_end_interrupt(irq);    
}

void arch_rwinvalid(unsigned int addr){
    kprintf("\r\nRWINVALID: %08x\r\n", addr);
    for(;;);
}
