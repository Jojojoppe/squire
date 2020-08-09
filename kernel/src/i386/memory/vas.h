#ifndef __VAS_H
#define __VAS_H 1
/**
 * @file
 * @section DESCRIPTION
 * The virtual address space manager
 */

// The ARCH interface
#include <general/arch/vas.h>
// The architecture configuration
#include <i386/config.h>

/**
 * @brief Initialize VAS
 * 
 * @return zero if successful
 */
int vas_init();

#endif