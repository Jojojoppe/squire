#include "config.h"
#include "../../common/kprintf.h"

void archmain(){
    // Initialzize kernel debug log
    kdebuglog_init();
    kprintf("\r\nSQUIRE ARM %s\r\n", VERSION);

}