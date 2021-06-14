#include "config.h"
#include "../../common/kprintf.h"
#include "pmm.h"
#include "vas.h"
#include "../../common/kmalloc.h"

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
}