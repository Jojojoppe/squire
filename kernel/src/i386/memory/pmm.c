#include <i386/memory/pmm.h>
#include <mboot.h>

static unsigned char pmm_map[0x20000];
static unsigned int pmm_mem_used;
static unsigned int pmm_mem_free;

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