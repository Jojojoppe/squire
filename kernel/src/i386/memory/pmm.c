#include <i386/memory/pmm.h>
#include <general/mboot.h>

int pmm_mboot_get_mmap(mboot_info_t * mboot_info, mboot_mmap_t ** previous, void ** base, unsigned long * length);
void pmm_unuse(unsigned int pagenum, unsigned int amount);
void pmm_use(unsigned int pagenum, unsigned int amount);

static unsigned char * pmm_mem_map[0x20000];
static unsigned int pmm_mem_free;

int pmm_init(void * bootinfo){
    mboot_info_t * mboot_info = (mboot_info_t*)bootinfo;

    // Traverse mboot mmap
    mboot_mmap_t * mmap_entry = 0;
    void * base = 0;
    unsigned long length = 0;
    do{
        if(pmm_mboot_get_mmap(mboot_info, &mmap_entry, &base, &length)){
            return -1;
        }
        if(length && mmap_entry){
            // Useful region found
            pmm_unuse((unsigned int)base/4096, length/4096);
        }
    }while(mmap_entry);

    // Set memory used kernel as used
    // For simplicity use 4MiB kernel size
    pmm_use(0x100000/4096, 4*1024*1024/4096);

    debug_print_sd("Memory free (B): ", pmm_mem_free);
    debug_print_sd("Memory free (KiB): ", pmm_mem_free/1024);
    debug_print_sd("Memory free (MiB): ", pmm_mem_free/1024/1024);

    return 0;
}

int pmm_alloc(unsigned int length, void ** address){
    // Check if correct length
    if(!length)
        return -1;
    // Check if enough free space
    if(length>pmm_mem_free)
        return -1;
    
    int pages = length/4096;
    int cont = 0;
    int start = 0;
    int streak = 0;
    for(int i=0; i<0x20000; i++){
        // If byte is zero all pages are used
        if(!pmm_mem_map[i])
            continue;
        for(int j=0; j<8; j++){
            if(((unsigned int)pmm_mem_map[i]>>j)&0x01){
                // Found free bit
                cont++;
                if(!streak){
                    streak = 1;
                    start = i*8+j;
                }
            }else{
                streak = 0;
                cont = 0;
            }
            if(cont==pages){
                // Found correct streak
                *address = start*4096;
                pmm_use(start, cont);
                return 0;
            }
        }
    }
    return -1;
}

int pmm_allocs(unsigned int length, void * address){
    // Check if correct length
    if(!length)
        return -1;
    // Check if enough free space
    if(length>pmm_mem_free)
        return -1;

    int pages = length/4096;
    unsigned int page = (unsigned int)address/4096;

    for(int i=0; i<pages; i++){
        int byte = (page+i)/8;
        int bit = (page+i)%8;
        if(!((unsigned int)pmm_mem_map[byte]>>bit)&0x01){
            return -1;
        }
    }

    pmm_use(page, pages);

    return 0;
}

int pmm_free(unsigned int length, void * address){
    pmm_unuse((unsigned int)address/4096, length/4096);
    return 0;
}

int pmm_mboot_get_mmap(mboot_info_t * mboot_info, mboot_mmap_t ** previous, void ** base, unsigned long * length){
    // Check if first time called
    if(!*previous){
        // Check if mmap is present
        if(!(mboot_info->flags&(1<<6))){
            // Not present: ERROR
            return -1;
        }
        // Load address of first entry
        *previous = (mboot_mmap_t*)(mboot_info->memMapAddress+KERNEL_VIRTUAL_BASE);
    }else{
        *previous = (mboot_mmap_t*)((void*)*previous + (*previous)->size + 4);
    }
    // Check if last
    if(mboot_info->memMapAddress + mboot_info->memMapLength + KERNEL_VIRTUAL_BASE<=(void*)(*previous)){
        // Last entry
        *previous = 0;
        return 0;
    }
    *base = (void*)((*previous)->base);
    // Check type of region
    // Since on i386 only low halfs of address are used
    if((*previous)->type==1){
        *length = (*previous)->length;
    }else{
        *length = 0;
    }
    return 0;
}

void pmm_unuse(unsigned int pagenum, unsigned int amount){
    for(int i=0; i<amount; i++){
        // Get index of page
        int index = pagenum>>5;
        // Get bit index of page
        int bit = pagenum&0x1f;
        ((unsigned int*)pmm_mem_map)[index] |= 1<<bit;
        pmm_mem_free += 4096;
        pagenum++;
    }
}

void pmm_use(unsigned int pagenum, unsigned int amount){
    for(int i=0; i<amount; i++){
        // Get index of page
        int index = pagenum>>5;
        // Get bit index of page
        int bit = pagenum&0x1f;
        ((unsigned int*)pmm_mem_map)[index] &= ~(1<<bit);
        pmm_mem_free -= 4096;
        pagenum++;
    }
}

