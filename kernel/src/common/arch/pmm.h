#ifndef __H_COMMON_ARCH_PMM
#define __H_COMMON_ARCH_PMM 1

#include <stddef.h>

/*
 * Type of memory
 */
typedef enum{
    PMM_MEMORY_TYPE_NORMAL,     /* Normal RAM */
    PMM_MEMORY_TYPE_FAST,       /* Fast RAM */
    PMM_MEMORY_TYPE_MISC,       /* Other types of memory */
    PMM_MEMORY_TYPE_UNUSABLE,   /* Unusable memory */
    _PMM_MEMORY_TYPE_SIZE,
} pmm_memory_type_t;

extern const size_t arch_pmm_framesize;

/*
 * Allocate amount of frames of specific type
 */
extern void * arch_pmm_alloc(unsigned int frames, pmm_memory_type_t type);

/*
 * Free amount of allocated frames
 */
extern void arch_pmm_free(void * base, unsigned int frames);

/*
 * Set amount of frames as used without allocating them
 */
extern void arch_pmm_use(void * base, unsigned int frames);

/*
 * Set amount of frames as unused without freeing them
 */
extern void arch_pmm_unuse(void * base, unsigned int frames);

/*
 * Get amount of available memory
 */
extern size_t arch_pmm_get_available();

/*
 * Get amount of free memory
 */
extern size_t arch_pmm_get_free();

/*
 * Get amount of used memory
 */
extern size_t arch_pmm_get_used();

#endif