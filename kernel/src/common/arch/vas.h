#ifndef __H_COMMON_ARCH_VAS
#define __H_COMMON_ARCH_VAS 1

#include <stddef.h>

#define VAS_FLAGS_COW 1
#define VAS_FLAGS_AOA 2

#define VAS_FLAGS_KREAD 4
#define VAS_FLAGS_KWRITE 8
#define VAS_FLAGS_KEXEC 16

#define VAS_FLAGS_UREAD 32
#define VAS_FLAGS_UWRITE 64
#define VAS_FLAGS_UEXEC 128


/*
 * Map a physical page to a virtual page
 */
extern int arch_vas_map(void * physical, void * virtual, unsigned int flags);

/*
 * Unmap a virtual page
 */
extern int arch_vas_unmap(void * virtual);

#endif