#ifndef __H_ARCH_PMM
#define __H_ARCH_PMM 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent PMM
 */

#include <general/stdint.h>

/**
 * @brief Allocate region in physical memory
 * 
 * @param length Length of region
 * @param address Address of variable to store start of region
 * @return zero if successfull
 */
extern int pmm_alloc(size_t length, void ** address);

/**
 * @brief Allocate specific region in physical memory
 * 
 * @param length Length of region
 * @param address Start of region
 * @return zero if successfull
 */
extern int pmm_allocs(size_t length, void * address);

/**
 * @brief Free a region in physical memory
 * 
 * @param lenght Length of region
 * @param address Start of region
 * @return zero if successfull
 */
extern int pmm_free(size_t lenght, void * address);

#endif