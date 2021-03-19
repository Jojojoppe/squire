#ifndef __H_COMMON_ARCH_DEBUGPRINT
#define __H_COMMON_ARCH_DEBUGPRINT 1

/*
 * Print a character to the debug print output
 */
extern void arch_debugprint_putc(char c);

/*
 * Initialize debug print output
 */
extern void arch_debugprint_init();

#endif
