#include "config.h"
#include "../../common/kprintf.h"

void archmain(){
    // Initialzize kernel debug log
    kdebuglog_init();
    kprintf("SQUIRE I386 %s\r\n", VERSION);
}