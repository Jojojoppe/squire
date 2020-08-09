#include <general/kmalloc.h>
#include <general/arch/vas.h>

static heap_block_t * heap_start;
unsigned int heap_size;

int kmalloc_init(){
    // Create start of heap by allocating 16KiB
    heap_start = (heap_block_t*) vas_brk(0x4000);
    heap_start->next = 0;
    // Create first chunk header
    heap_chunk_t * chunk = (heap_chunk_t*)(heap_start+1);
    chunk->length = 0x4000 - sizeof(heap_block_t) - sizeof(heap_chunk_t);
    chunk->flags = 1<<HEAP_CUNK_LAST;
    return 0;
}