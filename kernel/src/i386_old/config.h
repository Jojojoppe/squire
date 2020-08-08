#ifndef __I386_CONFIG_H
#define __I386_CONFIG_H 1
/**
 * @file
 * @section DESCRIPTION
 * The main config file of the i386 architecture
 */

#include <arch/config.h>

#undef ARCH
#define ARCH "i386"

#define KERNEL_VIRTUAL_BASE 0xc0000000

#endif