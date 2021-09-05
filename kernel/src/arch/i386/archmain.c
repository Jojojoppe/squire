#include "config.h"
#include "../../common/kprintf.h"
#include "pmm.h"
#include "vas.h"
#include "../../common/kmalloc.h"
#include "pic.h"
#include "idt.h"
#include "cpu.h"

void archmain(void * mboot_info, void * boot_heap_base){
    // Initialzize kernel debug log
    kdebuglog_init();
    kprintf("\r\nSQUIRE I386 %s\r\n", VERSION);

    // Initialize kernel heap
    kmalloc_init(boot_heap_base, BOOT_HEAP_LENGTH*0x1000);
    // Initialize page frame allocator
    arch_pmm_init(mboot_info);
    arch_vas_init();

    // Print memory information
    kprintf("Amount of available memory:    %d Kb\r\n", arch_pmm_get_available()/1024);
    kprintf("Amount of free memory:         %d Kb\r\n", arch_pmm_get_free()/1024);
    kprintf("Amount of used memory:         %d Kb\r\n", arch_pmm_get_used()/1024);

	// Create and initialize IDT
	idt_init();
	// Initialize and remap PIC
	pic_init();
	// Enable interrupts
	cpu_enable_interrupts();

    extern void main();
    main();
	for(;;);
}
