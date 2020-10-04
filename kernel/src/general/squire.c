#include <general/kprintf.h>
#include <general/arch/proc.h>
#include <general/arch/timer.h>
#include <general/kmalloc.h>
#include <general/config.h>

void squire_init2();

/**
 * @brief Main init function
 * 
 * When this function is called the following subsystems must be working:
 * - pmm
 * - vas
 * - putchar
 */
void squire_init(){
    printf("\r\n" \
        "+----------------+\r\n" \
        "|     SQUIRE     |\r\n" \
        "+----------------+\r\n" \
        "| a microkernel! |\r\n" \
        "+----------------+\r\n" \
        "version %s\r\n" \
        "compiled for: %s\r\n" \
    , SQUIRE_VERSION, SQUIRE_ARCH);

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

    for(;;){
        printf("\r[%08d] SQUIRE", timer_get());
    }
}