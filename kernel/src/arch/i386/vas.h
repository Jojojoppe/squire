#ifndef __H_VAS
#define __H_VAS 1

#include "../../common/arch/vas.h"

/*
 * Initialize virtual address space
 */
void arch_vas_init();

unsigned int arch_vas_getcr3();
unsigned int arch_vas_getpte(void * address);
unsigned int arch_vas_getpde(void * address);

#endif