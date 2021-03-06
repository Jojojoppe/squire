#ifndef __KMALLOC_H
#define __KMALLOC_H 1
/**
 * @file
 * @section DESCRIPTION
 * Kernel heap functions
 */

#include <general/stdint.h>

typedef struct heap_block_s{
    struct heap_block_s * next;
    size_t biggest;
    size_t size;
} heap_block_t;

#define HEAP_CHUNK_USED 1
#define HEAP_CHUNK_LAST 2
typedef struct heap_chunk_s{
    size_t length;
    unsigned int flags;
} heap_chunk_t;

#define HEAP_CHUNK_MINSIZE 8

/**
 * @brief Initializes the kernel heap
 * 
 * @return zero if successfull
 */
int kmalloc_init();

void * kmalloc(size_t length);

void kfree(void * address);

#endif