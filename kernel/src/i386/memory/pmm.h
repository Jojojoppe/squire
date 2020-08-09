#ifndef __PMM_H
#define __PMM_H 1
/**
 * @file
 * @section DESCRIPTION
 * The physical memory manager
 */

// The ARCH interface
#include <general/arch/pmm.h>
#include <general/config.h>

/**
 * @brief Initialize PMM
 * 
 * @param bootinfo MBoot header address
 * @return zero if successful
 */
int pmm_init(void * bootinfo);

#endif