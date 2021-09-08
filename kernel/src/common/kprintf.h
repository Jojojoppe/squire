#ifndef __H_KPRINTF
#define __H_KPRINTF 1

#include <stdarg.h>

/*
 * Flag which indicates if the kernel log is printed to a boot
 * debug device or if the kernel log is not printed to a visible
 * output at all.
 * 0 -> debug print to output disabled
 * others -> debug print to output enabled
 */
static unsigned char kernel_log_debug_out;

/*
 * Print kernel debug information to kernel log
 */ 
int kprintf(char * fmt, ...);

/*
 * Initialize kernel debug log
 * Must be called from the architecture specific boot code since it may be used there
 */
void kdebuglog_init();

void khexdump(void * addr, unsigned int length);

#endif
