#ifndef __H_VAS
#define __H_VAS 1

#include "../../common/arch/vas.h"

/*
 * Initialize virtual address space
 */
void arch_vas_init();

void * arch_vas_create_pt();

#endif