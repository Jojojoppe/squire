#include <stddef.h>
#include "../../../../common/arch/pmm.h"

const void * platform_mem_regions_base[] = {
    3,
    0x00000000,
    0xffff0000,
    0x00100000,
};

const size_t platform_mem_regions_length[] = {
    3,
    0x00030000,
    0x00001000,
    0x0ff00000,
};

const unsigned int platform_mem_regions_type[] = {
    3,
    PMM_MEMORY_TYPE_FAST,
    PMM_MEMORY_TYPE_FAST,
    PMM_MEMORY_TYPE_NORMAL,
};