#ifndef __VMM_H
#define __VMM_H 1
/**
 * @file
 * @section DESCRIPTION
 * The virtual memory manager
 */

#include <general/stdint.h>

#define VMM_FLAGS_USED 1
#define VMM_FLAGS_READ 2
#define VMM_FLAGS_WRITE 4
#define VMM_FLAGS_EXEC 8
typedef struct vmm_region_s{
    struct vmm_region_s * next;
    struct vmm_region_s * prev;
    void * base;
    size_t length;
    unsigned int flags;
} vmm_region_t;

/**
 * @brief Create a new virtual address space
 * 
 * @param base First block of the virtual address space
 * @return zero if successfull
 */
int vmm_create(vmm_region_t ** base);

/**
 * @brief Destroy a region list
 * 
 * Free all allocated memory whithin this address space
 * 
 * @param base First block of the virtual address space
 * @return zero if successfull
 */
int vmm_destroy(vmm_region_t ** base);

/**
 * @brief Allocate a memory region
 * 
 * Allocation will alter current page table
 * 
 * @param base Start of region
 * @param length Length of region
 * @param flags Region flags
 * @param list VMM region list
 * @return zero if successfull
 */
int vmm_alloc(void * base, size_t length, unsigned int flags, vmm_region_t ** list);

#endif