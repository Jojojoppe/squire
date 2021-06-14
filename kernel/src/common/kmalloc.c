#include "kmalloc.h"
#include "arch/spinlock.h"
#include "arch/pmm.h"

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

heap_block_t * heap_start;
unsigned int heap_size;
arch_spinlock_t heap_lock;

void kmalloc_init(void * base, size_t length){
    // Create start of heap by allocating 16KiB
    heap_start = (heap_block_t*) base;
    heap_start->next = 0;
    heap_start->biggest = length - sizeof(heap_block_t) - sizeof(heap_chunk_t);
    heap_start->size = heap_start->biggest;
    heap_size = length;
    // Create first chunk header
    heap_chunk_t * chunk = (heap_chunk_t*)(heap_start+1);
    chunk->length = heap_start->biggest;
    chunk->flags = HEAP_CHUNK_LAST;
	
	arch_spinlock_init(&heap_lock);
}

void kmalloc_clean(){
    // printf("kmalloc_clean()\r\n");
    heap_block_t * block = heap_start;
    while(block){
        heap_chunk_t * chunk = (heap_chunk_t*)(block+1);
        heap_chunk_t * prev = 0;        // If this is non-zero it could be merged with the current one if possible
        size_t biggest = 0;
        while(chunk){
            // Check if current one is free
            if((chunk->flags&HEAP_CHUNK_USED)==0){
                // If there is a previous one set: MERGE!
                if(prev){
                    prev->length += sizeof(heap_chunk_t) + chunk->length;
                    prev->flags = chunk->flags;
                    chunk = prev;
                }else{
                    // No merge possible yet, but maybe next round
                    prev = chunk;
                }
                // Update biggest if needed
                if(biggest<chunk->length){
                    biggest = chunk->length;
                }
            }else{
                prev= 0;
            }

            // goto next
            if((chunk->flags&HEAP_CHUNK_LAST)==0){
                chunk = (heap_chunk_t*)((void*)(chunk+1) + chunk->length);
                continue;
            }
            break;
        }

        block->biggest = biggest;

        // goto next
        if(block->next != 0){
            block = block->next;
            continue;
        }
        // No next block
        break;
    }
}

void * kmalloc(size_t length){
	arch_spinlock_lock(&heap_lock);

    // printf("kmalloc(%08x)\r\n", length);
    heap_block_t * block = heap_start;
    while(block){
        // Check if block is usable
        if(block->biggest>=length){
            heap_chunk_t * chunk = (heap_chunk_t*)(block+1);
            while(chunk){
                // Check if chunk is usable
                if((chunk->flags&HEAP_CHUNK_USED)==0 && chunk->length>=length){
                    // Check if chunk can be split
                    int rest = (int)chunk->length - length - sizeof(heap_chunk_t);
                    if(rest>HEAP_CHUNK_MINSIZE){
                        // Splitting chunk
                        heap_chunk_t * newchunk = (heap_chunk_t*)((void*)(chunk+1) + length);
                        newchunk->flags = chunk->flags;
                        // Now it is known current chunk is not last
                        chunk->flags &= ~HEAP_CHUNK_LAST;
                        newchunk->length = chunk->length-(length+sizeof(heap_chunk_t));
                        chunk->length = length;
                    }
                    chunk->flags |= HEAP_CHUNK_USED;
                    kmalloc_clean();
					arch_spinlock_unlock(&heap_lock);
                    return (void*)(chunk+1);
                }
                // Chunk not usable, goto next
                if((chunk->flags&HEAP_CHUNK_LAST)==0){
                    chunk = (heap_chunk_t*)((void*)(chunk+1) + chunk->length);
                    continue;
                }
                break;
            }
        }
        // return 0;
        // Block not usable, goto next
        if(block->next){
            block = block->next;
            continue;
        }
        /*
        // No next block! Create a new one
        size_t newlen = length+sizeof(heap_block_t) + sizeof(heap_chunk_t);
        newlen += arch_pmm_framesize - (newlen&0x3fff);
        heap_block_t * newblock = vas_brk(newlen);              // TODO implement vas_brk
        block->next = newblock;
        newblock->biggest = newlen-sizeof(heap_block_t)-sizeof(heap_chunk_t);
        newblock->size = newblock->biggest;
        // Create a chunk
        heap_chunk_t * chunk = (heap_chunk_t*)(newblock+1);
        chunk->length = newblock->biggest;
        chunk->flags = HEAP_CHUNK_LAST;

        block = newblock;
        */
        break;
    }
	arch_spinlock_unlock(&heap_lock);
    return NULL;
}

void kfree(void * base){
	arch_spinlock_lock(&heap_lock);
    // Get chunk header
    heap_chunk_t * chunk = (heap_chunk_t*)(base - sizeof(heap_chunk_t));
    chunk->flags &= ~HEAP_CHUNK_USED;
    kmalloc_clean();
	arch_spinlock_unlock(&heap_lock);
}