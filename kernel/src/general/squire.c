#include <general/kprintf.h>
#include <general/arch/proc.h>
#include <general/arch/timer.h>
#include <general/kmalloc.h>
#include <general/schedule.h>
#include <general/mboot.h>
#include <general/elf.h>
#include <general/config.h>
#include <general/message.h>
#include <general/string.h>

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
    printf("- initramfs.tar at %08x\r\n", initramfs_address);

    // Create user stack
    // TODO addresses are hardcoded here
    printf("- Create user stack\r\n");
    vmm_region_t * proc_mem = proc_get_memory();
    vmm_alloc(0xbfffc000,0x4000,VMM_FLAGS_READ|VMM_FLAGS_WRITE,&proc_mem);

    // Copy initramfs.tar to user space
    vmm_alloc(0x50000000, (initramfs_length/4096+1)*4096, VMM_FLAGS_READ, &proc_mem);
    proc_set_memory(proc_mem);
    memcpy(0x50000000, initramfs_address, initramfs_length);

    // Send init.bin params as messages
    // First argc with length of data
    unsigned int params_data1[2] = {2, 9+sizeof(void*)+2*sizeof(unsigned int)};
    message_simple_send(1, sizeof(unsigned int)*2, params_data1);
    unsigned int * params_data2 = (unsigned int*) kmalloc(params_data1[1]);
    params_data2[0] = 9;
    params_data2[1] = sizeof(void*);
    strcpy(params_data2+2, "init.bin");
    *((unsigned int*)((char*)(params_data2+2)+9)) = 0x50000000;
    message_simple_send(1, params_data1[1], params_data2);
    kfree(params_data2);

    proc_thread_new_user(init_entry, 0xbfffc000, 0x4000, proc_proc_get_current());
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