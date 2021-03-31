#ifndef __H_KMALLOC
#define __H_KMALLOC 1

#include <stddef.h>

/*
 * Allocate memory on kernel heap
 */
void * kmalloc(size_t length);

/*
 * Free memory from kernel heap
 */
void kfree(void * base);

/*
 * Initialize kernel heap
 */
void kmalloc_init(void * base, size_t length);

#endif