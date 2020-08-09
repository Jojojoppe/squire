#ifndef __I386_CONFIG_H
#define __I386_CONFIG_H 1
/**
 * @file
 * @section DESCRIPTION
 * The main config file of the i386 architecture
 */

#include <general/arch/config.h>

#define KERNEL_VIRTUAL_BASE 0xc0000000

// IO FUNCTIONS
extern void io_outb(unsigned int address, unsigned char b);
extern void io_outw(unsigned int address, unsigned short b);
extern void io_outd(unsigned int address, unsigned int b);
extern unsigned char io_inb(unsigned int address);
extern unsigned short io_inw(unsigned int address);
extern unsigned int io_ind(unsigned int address);

// DEBUG FUNCTIONS
extern void debug_print_s(char * msg);
extern void debug_print_x(unsigned int num);
extern void debug_print_d(unsigned int num);
extern void debug_print_sx(char * name, unsigned int num);
extern void debug_print_sd(char * name, unsigned int num);

#endif