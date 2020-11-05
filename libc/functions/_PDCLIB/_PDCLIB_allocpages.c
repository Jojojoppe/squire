#include <stdint.h>
#include <stddef.h>
#include "_PDCLIB_glue.h"
#include <errno.h>

#include <squire.h>

char * heap_end = 0;
void * _PDCLIB_allocpages( size_t n ){
    /*
	extern char _end;

    unsigned int incr = n*_PDCLIB_MALLOC_PAGESIZE;

    char * prev_heap_end;
    if(heap_end==0){
        heap_end = (char*)((((unsigned int)&_end)/4096+1)*4096);
    }
    prev_heap_end = heap_end;
    heap_end += (incr/4096+1)*4096;

    squire_memory_mmap(prev_heap_end, (incr/4096+1)*4096, MMAP_READ | MMAP_WRITE);
    return (void *) prev_heap_end;
	*/

	return squire_memory_mmap(0, n*_PDCLIB_MALLOC_PAGESIZE, MMAP_READ|MMAP_WRITE);
}
