#include "config.h"
#include "../../common/kprintf.h"

void archmain(){
    // Initialzize kernel debug log
    kdebuglog_init();
    kprintf("SQUIRE X86_64 %s\r\n", VERSION);
}