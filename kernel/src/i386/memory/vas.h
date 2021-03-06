#ifndef __VAS_H
#define __VAS_H 1
/**
 * @file
 * @section DESCRIPTION
 * The virtual address space manager
 */

// The ARCH interface
#include <general/arch/vas.h>
#include <general/config.h>

/**
 * @brief Initialize VAS
 * 
 * @return zero if successful
 */
int vas_init();

unsigned int vas_getcr3();

int vas_pagefault(void * addr, unsigned int error);

unsigned int vas_get_pte(void * addr);

#define KERNEL_PT 0xffc00000
#define KERNEL_PD 0xfffff000

#endif