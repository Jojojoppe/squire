#ifndef __H_ARCH_PMM
#define __H_ARCH_PMM 1
/**
 * @file
 * @section DESCRIPTION
 * Defines the interface of the architecture dependent PMM
 */


/**
 * @brief Initialize PMM
 * 
 * @param bootinfo Architecture specific boot information (like MBOOT)\
 * @return zero if successfull 
 */
extern int pmm_init(void * bootinfo);

/**
 * @brief Allocate region in physical memory
 * 
 * @param length Length of region
 * @param address Address of variable to store start of region
 * @return zero if successfull
 */
extern int pmm_alloc(unsigned int length, void ** address);

/**
 * @brief Allocate specific region in physical memory
 * 
 * @param length Length of region
 * @param address Start of region
 * @return zero if successfull
 */
extern int pmm_allocs(unsigned int length, void * address);

/**
 * @brief Free a region in physical memory
 * 
 * @param lenght Length of region
 * @param address Start of region
 * @return zero if successfull
 */
extern int pmm_free(unsigned int lenght, void * address);

#endif