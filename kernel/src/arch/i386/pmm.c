#include "pmm.h"
#include "../../common/kprintf.h"
#include "mboot.h"
#include "../../common/arch/spinlock.h"
#include "../../common/kmalloc.h"

const size_t arch_pmm_framesize = 4096;

// The complete 4GB memory bitmap specifying the pages of arch_pmm_framesize big
static unsigned char arch_pmm_memory_bitmap[0x20000];

size_t arch_pmm_memory_free = 0;
size_t arch_pmm_memory_used = 0;
size_t arch_pmm_memory_available = 0;

typedef struct pmm_region_s{
    void * start;
    size_t length;
    struct pmm_region_s * next;
} pmm_region_t;

// A set of linked lists for each type
pmm_region_t pmm_regions[_PMM_MEMORY_TYPE_SIZE];

// PMM structure lock
arch_spinlock_t pmm_lock;

void arch_pmm_init(void * mboot_info){
    kprintf("      Initializing PMM\r");

    // Clean pmm region list
    for(int i=0; i<_PMM_MEMORY_TYPE_SIZE; i++){
        pmm_regions[i].start = 0;
        pmm_regions[i].length = 0;
        pmm_regions[i].next = 0;
    }

    arch_pmm_memory_available = 0;
    arch_pmm_memory_free = 0;
    arch_pmm_memory_used = 0;

    // Traverse mboot map
    mboot_info_t * mboot_header = (mboot_info_t*)mboot_info;
    mboot_mmap_t * mmap_entry = 0;
    void * base = 0;
    unsigned long length = 0;
    do{
        if(mboot_get_mmap(mboot_header, &mmap_entry, &base, &length)){
            kprintf("[ER]\r\nERROR: no MBOOT memory info\r\n");
            // TODO PANIC
            for(;;);
        }
        if(mmap_entry){
            // type:
            //   - 1 : usable normal RAM
            //   - 3 : ACPI information
            //   - 4 : reserved memory to be preserved on hibernation
            //   - 5 : Defective RAM modules
            if(mmap_entry->type==1){
                // usable RAM: TYPE_NORMAL
                arch_pmm_memory_available += length;
                // Add to region list
                if(pmm_regions[PMM_MEMORY_TYPE_NORMAL].length==0){
                    // Can add info to the first region
                    pmm_regions[PMM_MEMORY_TYPE_NORMAL].start = base;
                    pmm_regions[PMM_MEMORY_TYPE_NORMAL].length = length;
                    pmm_regions[PMM_MEMORY_TYPE_NORMAL].next = 0;
                }else{
                    // Create new block and link
                    pmm_region_t * r = &pmm_regions[PMM_MEMORY_TYPE_NORMAL];
                    while(r->next) r=r->next;
                    r->next = (pmm_region_t*) kmalloc(sizeof(pmm_region_t));
                    if(r->next==NULL){
                        kprintf("[ER]\r\nERROR: Could not allocate new region descriptor\r\n");
                        // TODO PANIC
                        for(;;);
                    }
                    r->next->start = base;
                    r->next->length = length;
                    r->next->next = 0;
                }
            }else if(mmap_entry->type==3){
                // ACPI info: TYPE_MISC
                // Add to region list
                if(pmm_regions[PMM_MEMORY_TYPE_MISC].length==0){
                    // Can add info to the first region
                    pmm_regions[PMM_MEMORY_TYPE_MISC].start = base;
                    pmm_regions[PMM_MEMORY_TYPE_MISC].length = length;
                    pmm_regions[PMM_MEMORY_TYPE_MISC].next = 0;
                }else{
                    // Create new block and link
                    pmm_region_t * r = &pmm_regions[PMM_MEMORY_TYPE_MISC];
                    while(r->next) r=r->next;
                    if(r->next==NULL){
                        kprintf("[ER]\r\nERROR: Could not allocate new region descriptor\r\n");
                        // TODO PANIC
                        for(;;);
                    }
                    r->next = (pmm_region_t*) kmalloc(sizeof(pmm_region_t));
                    r->next->start = base;
                    r->next->length = length;
                    r->next->next = 0;
                }
            }
        }
    } while(mmap_entry);

    arch_spinlock_init(&pmm_lock);

    // Clear bitmap and set to unused for all regions
    for(int i=0; i<0x20000; i++){
        arch_pmm_memory_bitmap[i] = 0xff;
    }

    arch_pmm_memory_free = 0;
    arch_pmm_memory_used = arch_pmm_memory_available;

    pmm_region_t * r = &pmm_regions[PMM_MEMORY_TYPE_NORMAL];
    while(r){
        arch_pmm_unuse(r->start, r->length/arch_pmm_framesize);
        r = r->next;
    }

    // Block off kernel memory
    // For simplicity use 4MiB kernel size
    arch_pmm_use(0x100000/arch_pmm_framesize, 4*1024*1024/arch_pmm_framesize);

    kprintf("[OK]\r\n");
}

void * arch_pmm_alloc(unsigned int frames, pmm_memory_type_t type){

    if(frames==0 || type>=_PMM_MEMORY_TYPE_SIZE){
        return NULL;
    }

    size_t size = frames*arch_pmm_framesize;

    // Check if enough memory left
    if(size>arch_pmm_memory_free){
        return NULL;
    }

    // Find free memory region
    pmm_region_t * reg = &pmm_regions[type];
    while(reg){

        // Check if region is big enough
        if(reg->length>=size){
            // Traverse region to find free memory
            unsigned int byte = (unsigned int)reg->start/(arch_pmm_framesize*8);
            unsigned int bit = (unsigned int)reg->start&0x07;
            unsigned int start = 0;
            unsigned int streak = 0;
            for(int i=0; i<reg->length/arch_pmm_framesize; i++){

                if((arch_pmm_memory_bitmap[byte]&(1<<bit))==0){
                    // Free frame found;
                    if(streak==0){
                        // First free frame, save start
                        start = byte*8*arch_pmm_framesize+bit;
                    }
                    streak++;
                    if(streak==frames){
                        arch_pmm_use(start, frames);
                        return (void *)start;
                    }
                }else{
                    // Used frame found
                    streak = 0;
                }

                bit++;
                if(bit>=8){
                    bit=0;
                    byte++;
                }
            }
        }

        reg = reg->next;
    }

    return NULL;
}

void arch_pmm_free(void * base, unsigned int frames){
    arch_pmm_unuse(base, frames);
}

void arch_pmm_use(void * base, unsigned int frames){
    // Calculate offset and length in bitmap
    unsigned int offset = (unsigned int)base/(arch_pmm_framesize*8);
    unsigned int bitoffset = (unsigned int)base & 0x07;

    if(frames==0){
        return;
    }

    arch_spinlock_lock(&pmm_lock);

    arch_pmm_memory_used += frames*arch_pmm_framesize;
    arch_pmm_memory_free -= frames*arch_pmm_framesize;

    for(int i=0; i<frames; i++){
        arch_pmm_memory_bitmap[offset] |= 1<<bitoffset;
        bitoffset++;
        if(bitoffset==8){
            bitoffset = 0;
            offset++;
        }
    }
    arch_spinlock_unlock(&pmm_lock);
}

void arch_pmm_unuse(void * base, unsigned int frames){
    unsigned int offset = (unsigned int)base/(arch_pmm_framesize*8);
    unsigned int bitoffset = (unsigned int)base & 0x07;

    if(frames==0){
        return;
    }

    arch_spinlock_lock(&pmm_lock);

    arch_pmm_memory_used -= frames*arch_pmm_framesize;
    arch_pmm_memory_free += frames*arch_pmm_framesize;

    for(int i=0; i<frames; i++){
        arch_pmm_memory_bitmap[offset] &= ~(1<<bitoffset);
        bitoffset++;
        if(bitoffset==8){
            bitoffset = 0;
            offset++;
        }
    }
    arch_spinlock_unlock(&pmm_lock);
}

size_t arch_pmm_get_available(){
    return arch_pmm_memory_available;
}

size_t arch_pmm_get_free(){
    return arch_pmm_memory_free;
}

size_t arch_pmm_get_used(){
    return arch_pmm_memory_used;
}