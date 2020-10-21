#include <general/vmm.h>
#include <general/kmalloc.h>
#include <general/arch/vas.h>
#include <general/arch/pmm.h>
#include <general/config.h>

int vmm_create(vmm_region_t ** base){

    // Create the first block
    *base = kmalloc(sizeof(vmm_region_t));
    (*base)->base = VMM_USERREGION_BASE;
    (*base)->length = VMM_USERREGION_LENGTH;
    (*base)->flags = 0;
    (*base)->next = 0;
    (*base)->prev = 0;

    return 0;
}

int vmm_destroy(vmm_region_t ** base){
    vmm_region_t * region = *base;
    while(region){
        // Check if memory must be freed
        if((region->flags&VMM_FLAGS_USED)!=0){
            // Free all pages if not shared
            // TODO check for shared region
            for(int i=0; i<region->length/PAGE_SIZE; i++){
                vas_unmap_free(region->base + i*PAGE_SIZE);
            }
        }
        vmm_region_t * next = region->next;
        if(next){
            kfree(region);
            region = next;
        }
        break;
    }

    // Free base block
    kfree(*base);
    *base = 0;
    return 0;
}

int vmm_alloc(void * base, size_t length, unsigned int flags, vmm_region_t ** list){
    if(!length)
        return -1;
    // printf("vmm_alloc(%08x, %08x) @ %08x\r\n", base, length, list);

    // Find the descriptor of the region
    vmm_region_t * region = *list;
    while(region){
        // printf("region?: %08x [%08x] -- %08x [%08x]\n", region->base, region->length, base, length);
        if((size_t)region->base<=(size_t)base && (size_t)region->base+region->length>(size_t)base){
            // printf("Usable\n");
            // Within this current region
            // Check if free
            if((region->flags&VMM_FLAGS_USED)!=0){
                // printf("Already in use\n");
                // Already in use
                return -1;
            }
            // Check if within region bounds (if request is not to large)
            if((size_t)region->base+region->length<(size_t)base+length){
                // printf("Not within bounds\n");
                // Request to large
                return -1;
            }
            // Region usable
            // Check if new region must be made before new one
            if((size_t)base>(size_t)region->base){
                // printf("Make preregion\n");
                // Create new region before current one
                vmm_region_t * preregion = kmalloc(sizeof(vmm_region_t));
                preregion->base = region->base;
                preregion->flags = region->flags;
                preregion->length = (size_t)base - (size_t)region->base;
                preregion->next = region;
                preregion->prev = region->prev;
                if(preregion->prev){
                    preregion->prev->next = region;
                }
                region->prev = preregion;
                region->length -= preregion->length;
                region->base = base;
                // printf("preregion %08x [%08x]\n", preregion->base, preregion->length);
            }
            // Check if new region must be made after new one
            if(length<region->length){
                // printf("Make postregion\n");
                // Create new region after current one
                vmm_region_t * postregion = kmalloc(sizeof(vmm_region_t));
                postregion->base = (size_t)region->base + length;
                postregion->flags = region->flags;
                postregion->length = region->length - length;
                postregion->next = region->next;
                postregion->prev = region;
                if(postregion->next){
                    postregion->next->prev = postregion;
                }
                region->next = postregion;
                region->length = length;
                // printf("postregion %08x [%08x]\n", postregion->base, postregion->length);
            }else{
                region->next = 0;
            }
            
            // printf("Allocate region\n");
            // Allocate new region
            // Set flags
            region->flags = VMM_FLAGS_USED | flags;
            // Allocate memory
            unsigned int vas_flags = 0;
            if(flags&VMM_FLAGS_READ)
                vas_flags |= VAS_FLAGS_READ;
            if(flags&VMM_FLAGS_WRITE)
                vas_flags |= VAS_FLAGS_WRITE;
            void * physical_mem;
            pmm_alloc(length, &physical_mem);
            for(int i=0; i<length/PAGE_SIZE; i++){
                vas_map(physical_mem + i*PAGE_SIZE, base + i*PAGE_SIZE, vas_flags);
            }

            // Check if list is prepended
            while((*list)->prev){
                // printf("Set prepend\r\n");
                *list = (*list)->prev;
            }
            return 0;
        }

        // To next region
        if(region->next){
            region = region->next;
            continue;
        }
        // No more regions
        return -1;
    }

    return 0;
}

void vmm_debug(vmm_region_t * list){
    vmm_region_t * region = list;
    while(region){

        printf("region [%08x]:\n", region);
        printf(" - base:    %08x\n", region->base);
        printf(" - length:  %08x\n", region->length);
        printf(" - flags:   %08x\n", region->flags);
        printf(" - next:    %08x\r\n", region->next);

        // To next region
        if(region->next && region->next!=region){
            region = region->next;
            continue;
        }
        // No more regions
        break;
    }
}
