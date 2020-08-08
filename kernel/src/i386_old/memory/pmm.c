#include <i386/memory/pmm.h>
#include <mboot.h>



int pmm_init(void * bootinfo){
    mboot_info_t * mboot_info = (mboot_info_t*)(bootinfo+KERNEL_VIRTUAL_BASE);
    return 0;
}

int pmm_alloc(unsigned int length, void ** address){
    return 0;
}

int pmm_allocs(unsigned int lengh, void * address){
    return 0;
}

int pmm_free(unsigned int length, void * address){
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
    if((*previous)->type==1){
        *length = (*previous)->length;
    }else{
        *length = 0;
    }
}