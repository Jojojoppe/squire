#include <general/kprintf.h>
#include <general/arch/proc.h>
#include <general/arch/timer.h>
#include <general/kmalloc.h>
#include <general/schedule.h>
#include <general/mboot.h>
#include <general/elf.h>
#include <general/config.h>

void squire_init2();
void error(const char *);

/**
 * @brief MBOOT header address
 * 
 */
mboot_info_t * MBOOT_header;
/**
 * @brief MBOOT header address offset
 * 
 */
unsigned int MBOOT_offset;

/**
 * @brief Main init function
 * 
 * When this function is called the following subsystems must be working:
 * - pmm
 * - vas
 * - putchar
 * 
 * The first parameter is the MBOOT header address with the second one the 
 * memory offset needed for all addresses in the header
 */
void squire_init(mboot_info_t * header, unsigned int offset){
    printf("\r\n" \
        "+----------------+\r\n" \
        "|     SQUIRE     |\r\n" \
        "+----------------+\r\n" \
        "| a microkernel! |\r\n" \
        "+----------------+\r\n" \
        "version %s\r\n" \
        "compiled for: %s\r\n\r\n" \
        "MBOOT header at %08x [offset %08x]\r\n" \
    , SQUIRE_VERSION, SQUIRE_ARCH, header, offset);

    MBOOT_header = header;
    MBOOT_offset = offset;

    // Initialize kernel heap
    kmalloc_init();

    // Initialize processing
    proc_init(squire_init2);
}

/**
 * @brief Rest of main init function
 * 
 * This function is called after the process intialization. From now on
 * squire is running in process 1 thread 1
 */
void squire_init2(){
    timer_init();
    
    printf("- Multitasking initialized\r\n");

    // Find init.bin
    void * init_address;
    size_t init_length;
    if(mboot_get_mod("init.bin", &init_address, &init_length))
        error("Could not find init.bin in MBOOT modules");
    printf("- init.bin found\r\n");

    // Find initramfs.tar
    void * initramfs_address;
    size_t initramfs_length;
    if(mboot_get_mod("initramfs.tar", &initramfs_address, &initramfs_length))
        error("Could not find initramfs.tar in MBOOT modules");
    printf("- initramfs.tar found\r\n");

    // Load init.bin into memory
    void (*init_entry)();
    if(elf_load_simple(init_address, &init_entry))
        error("Could not load init.bin");

    printf("- Entry init.bin at %08x\r\n", init_entry);

    // Crrete user stack
    vmm_alloc(0xbfffc000,0x4000,VMM_FLAGS_READ|VMM_FLAGS_WRITE,proc_get_memory());

    proc_thread_new(init_entry, 0xc0000000-4, proc_proc_get_current());

    for(;;){
        printf("\r[%08d]\r", timer_get());
    }
}

/**
 * @brief Show error message and hang
 * 
 * @param msg Message to show
 */
void error(const char * msg){
    printf("\r\nERROR: %s\n", msg);
    schedule_disable();
    for(;;);
}