#ifndef __I386_CONFIG_H
#define __I386_CONFIG_H 1
/**
 * @file
 * @section DESCRIPTION
 * The main config file of the i386 architecture
 */

#include <general/arch/config.h>

#undef ARCH
#define ARCH "i386"

#define KERNEL_VIRTUAL_BASE 0xc0000000

// DEBUG FUNCTIONS
extern void debug_print_s(char * msg);
extern void debug_print_x(unsigned int num);
extern void debug_print_d(unsigned int num);
extern void debug_print_sx(char * name, unsigned int num);
extern void debug_print_sd(char * name, unsigned int num);

#endif