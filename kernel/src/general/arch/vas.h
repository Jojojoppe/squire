#ifndef __H_ARCH_VAS
#define __H_ARCH_VAS 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent VAS (virtual address space)
 */

#include <general/stdint.h>

#define VAS_FLAGS_READ 0
#define VAS_FLAGS_WRITE 2
// TODO flag for COW AOW?

/**
 * @brief Map a physical address page to a virtual page
 * 
 * @param physical Physical address to map
 * @param address Virtual address to map to
 * @param flags Page flags
 * @return zero if successfull
 */
extern int vas_map(void * physical, void * address, unsigned int flags);

/**
 * @brief Unmap a virtual page
 * 
 * @param address Virtual address to unmap
 * @return zero if successfull
 */
extern int vas_unmap(void * address);

/**
 * @brief Unmap a virtual page and free
 * 
 * @param address Virtual address to unmap
 * @return zero if successfull
 */
extern int vas_unmap_free(void * address);

/**
 * @brief Extend memory mapped to kernel
 * 
 * @param length Amount of memory to extend
 * @return void* Address of start of new region
 */
extern void * vas_brk(size_t length);

#endif