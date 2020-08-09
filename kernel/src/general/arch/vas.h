#ifndef __H_ARCH_VAS
#define __H_ARCH_VAS 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent VAS (virtual address space)
 */

/**
 * @brief Map a physical address page to a virtual page
 * 
 * @param physical Physical address to map
 * @param address Virtual address to map to
 * @return zero if successfull
 */
extern int vas_map(void * physical, void * address);

/**
 * @brief Unap a virtual page
 * 
 * @param address Virtual address to unmap
 * @return zero if successfull
 */
extern int vas_unmap(void * address);

/**
 * @brief Extend memory mapped to kernel
 * 
 * @param length Amount of memory to extend
 * @return void* Address of start of new region
 */
extern void * vas_brk(unsigned int length);

#endif