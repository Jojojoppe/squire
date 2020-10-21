#include <general/kprintf.h>
#include <general/arch/proc.h>
#include <general/arch/timer.h>
#include <general/kmalloc.h>
#include <general/schedule.h>
#include <general/config.h>

void testA(){
    for(;;){
        printf("\n\r[%08d] testA", timer_get());
    }
}

void testB(){
    for(;;){
        printf("\n\r[%08d] testB", timer_get());
    }
}


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

    schedule_disable();
        void * stackA = kmalloc(4096) + 4096 - 4;
        void * stackB = kmalloc(4096) + 4096 - 4;
        printf("create testA\r\n");
        proc_thread_new(testA, stackA, proc_proc_get_current());
        printf("create testB\r\n");
        proc_thread_new(testB, stackB, proc_proc_get_current());
        printf("created test tasks\r\n");
    schedule_enable();

    for(;;){
        printf("\n\r[%08d] SQUIRE", timer_get());
    }
}