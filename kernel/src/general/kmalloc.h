#ifndef __KMALLOC_H
#define __KMALLOC_H 1
/**
 * @file
 * @section DESCRIPTION
 * Kernel heap functions
 */

typedef struct heap_block_s{
    struct heap_block_s * next;
    unsigned int biggest;
    unsigned int size;
} heap_block_t;

#define HEAP_CHUNK_USED 0
#define HEAP_CUNK_LAST 1
typedef struct heap_chunk_s{
    unsigned int length;
    unsigned int flags;
} heap_chunk_t;

/**
 * @brief Initializes the kernel heap
 * 
 * @return zero if successfull
 */
int kmalloc_init();

#endif