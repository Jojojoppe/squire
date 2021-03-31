#ifndef __H_COMMON_ARCH_VAS
#define __H_COMMON_ARCH_VAS 1

#include <stddef.h>

#define VAS_FLAGS_READ 0
#define VAS_FLAGS_WRITE 2
#define VAS_FLAGS_COW 4
#define VAS_FLAGS_AOA 8

/*
 * Map a physical page to a virtual page
 */
extern int arch_vas_map(void * physical, void * virtual, unsigned int flags);

/*
 * Unmap a virtual page
 */
extern int arch_vas_unmap(void * virtual);

#endif