#include <general/kprintf.h>

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

    
}