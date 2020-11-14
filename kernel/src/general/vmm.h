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
#define VMM_FLAGS_PHYSICAL 16
#define VMM_FLAGS_SHARED 32
#define VMM_FLAGS_SHARED_TRANSFER 64
#define VMM_FLAGS_SHARED_TRANSFER_DISOWNED 128

typedef struct vmm_shared_s{
	struct vmm_shared_s * next;
	unsigned int flags;
	unsigned int owner;
	unsigned int shared_with;
	char id[32];
	void * phys_base;
	size_t phys_length;
} vmm_shared_t;

typedef struct vmm_region_s{
    struct vmm_region_s * next;
    struct vmm_region_s * prev;
    void * base;
    size_t length;
    unsigned int flags;
	vmm_shared_t * shared;
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
 * @brief Clean a region list
 *
 * Join consecutive regions with the same flags
 * @param base First block of the virtual address space
 */
void vmm_clean(vmm_region_t * base);

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

/**
 * @brief Allocate a region of memory, kernel may decide where
 *
 * @param base Pointer to variable which stores start of region
 * @param length Length of region
 * @param flags Region flags
 * @param list VMM region list
 * @return zero if successfull
 */
int vmm_alloc_auto(void ** base, size_t length, unsigned int flags, vmm_region_t ** list);

int vmm_map_phys(void * base, void * phys, size_t length, unsigned int flags, vmm_region_t ** list);

int vmm_map_phys_auto(void ** base, void * phys, size_t length, unsigned int flags, vmm_region_t ** list);

int vmm_create_shared(void * base, size_t length, unsigned int flags, char id[32], vmm_region_t ** list);

int vmm_create_shared_auto(void ** base, size_t length, unsigned int flags, char id[32], vmm_region_t ** list);

int vmm_map_shared(void * base, unsigned int flags, unsigned int owner, char id[32], vmm_region_t ** list);

int vmm_map_shared_auto(void ** base, unsigned int flags, unsigned int owner, char id[32], vmm_region_t ** list);

int vmm_transfer_shared(char id[32]);

int vmm_unmap(void * base, size_t length, vmm_region_t ** list);

#endif
