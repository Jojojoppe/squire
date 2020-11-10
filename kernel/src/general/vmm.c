#include <general/vmm.h>
#include <general/kmalloc.h>
#include <general/arch/vas.h>
#include <general/arch/pmm.h>
#include <general/config.h>
#include <general/arch/user_mem.h>
#include <general/arch/proc.h>
#include <general/string.h>

vmm_shared_t * vmm_shared_regions = 0;

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
            // TODO check for shared region or mapped physical page
			if((region->flags&VMM_FLAGS_PHYSICAL)!=0){
				for(int i=0; i<region->length/PAGE_SIZE; i++){
					vas_unmap_free(region->base + i*PAGE_SIZE);
				}
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

void vmm_clean(vmm_region_t * list){
	while(list){
		// Check if there is a next one
		if(list->next){
			// Check if it has the same flags
			if(list->flags == list->next->flags){
				// Merge current one with next one
				list->length += list->next->length;
				vmm_region_t * tmp = list->next;
				list->next = tmp->next;
				// Check if the new next one is there and edit the prev field of it
				if(list->next)
					list->next->prev = list;
				// Free cleaned up node
				kfree(tmp);
			}
		}

		list = list->next;
	}
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
                // printf("Current region: %08x\r\n", region);
                // Create new region before current one
                vmm_region_t * preregion = kmalloc(sizeof(vmm_region_t));
                preregion->base = region->base;
                preregion->flags = region->flags;
                preregion->length = (size_t)base - (size_t)region->base;
                preregion->next = region;
                preregion->prev = region->prev;
                if(region->prev){
                    region->prev->next = preregion;
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
            region->flags = VMM_FLAGS_USED | flags&(VMM_FLAGS_EXEC|VMM_FLAGS_READ|VMM_FLAGS_WRITE);
            // Allocate memory
            unsigned int vas_flags = 0;
            if(flags&VMM_FLAGS_READ)
                vas_flags |= VAS_FLAGS_READ;
            if(flags&VMM_FLAGS_WRITE)
                vas_flags |= VAS_FLAGS_WRITE;
            void * physical_mem;
            // pmm_alloc(length, &physical_mem);
            // for(int i=0; i<length/PAGE_SIZE; i++){
                // vas_map(physical_mem + i*PAGE_SIZE, base + i*PAGE_SIZE, vas_flags);
            // }
            vas_flags |= VAS_FLAGS_AOA;
            for(int i=0; i<length/PAGE_SIZE; i++){
                vas_map(0, base + i*PAGE_SIZE, vas_flags);
            }

            // Check if list is prepended
            if((*list)->prev){
                // printf("Set prepend\r\n");
                *list = (*list)->prev;
            }

			// Clean list
			vmm_clean(*list);
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

int vmm_alloc_auto(void ** base, size_t length, unsigned int flags, vmm_region_t ** list){
	if(!length)
		return -1;

	vmm_region_t * region = *list;
	while(region){

		// Check if region is unused and has enough size
		if(region->flags==0 && region->length>=length){
			// A usable region found, use it
			void * regbase = region->base;
			int status = vmm_alloc(region->base, length, flags, list);
			if(!status){
				*base = regbase;
				return 0;
			}
			// An error occured, try next region
		}

		region = region->next;
	}
	// Traversed all regions, no feasable found...
	return -1;
}

int vmm_map_phys(void * base, void * phys, size_t length, unsigned int flags, vmm_region_t ** list){
    if(!length)
        return -1;

	// Check if the region may be mapped
	if(user_mem_arch_may_map_phys(phys, length))
		return -1;
	
    // Find the descriptor of the region
    vmm_region_t * region = *list;
    while(region){
        if((size_t)region->base<=(size_t)base && (size_t)region->base+region->length>(size_t)base){
            // Within this current region
            // Check if free
            if((region->flags&VMM_FLAGS_USED)!=0){
                // Already in use
                return -1;
            }
            // Check if within region bounds (if request is not to large)
            if((size_t)region->base+region->length<(size_t)base+length){
                // Request to large
                return -1;
            }
            // Region usable
            // Check if new region must be made before new one
            if((size_t)base>(size_t)region->base){
                // Create new region before current one
                vmm_region_t * preregion = kmalloc(sizeof(vmm_region_t));
                preregion->base = region->base;
                preregion->flags = region->flags;
                preregion->length = (size_t)base - (size_t)region->base;
                preregion->next = region;
                preregion->prev = region->prev;
                if(region->prev){
                    region->prev->next = preregion;
                }
                region->prev = preregion;
                region->length -= preregion->length;
                region->base = base;
            }
            // Check if new region must be made after new one
            if(length<region->length){
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
            }else{
                region->next = 0;
            }
            
            // Allocate new region
            // Set flags
            region->flags = VMM_FLAGS_USED | flags&(VMM_FLAGS_EXEC|VMM_FLAGS_READ|VMM_FLAGS_WRITE) | VMM_FLAGS_PHYSICAL ;
            // Map the physical memory
            unsigned int vas_flags = 0;
            if(flags&VMM_FLAGS_READ)
                vas_flags |= VAS_FLAGS_READ;
            if(flags&VMM_FLAGS_WRITE)
                vas_flags |= VAS_FLAGS_WRITE;
            for(int i=0; i<length/PAGE_SIZE; i++){
                vas_map(phys+i*PAGE_SIZE, base + i*PAGE_SIZE, vas_flags);
            }

            // Check if list is prepended
            if((*list)->prev){
                *list = (*list)->prev;
            }

			// Clean list
			vmm_clean(*list);
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

int vmm_map_phys_auto(void ** base, void * phys, size_t length, unsigned int flags, vmm_region_t ** list){
	if(!length)
		return -1;

	vmm_region_t * region = *list;
	while(region){

		// Check if region is unused and has enough size
		if(region->flags==0 && region->length>=length){
			// A usable region found, use it
			void * regbase = region->base;
			int status = vmm_map_phys(region->base, phys, length, flags, list);
			if(!status){
				*base = regbase;
				return 0;
			}
			// An error occured, try next region
		}

		region = region->next;
	}
	// Traversed all regions, no feasable found...
	return -1;
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


int vmm_create_shared(void * base, size_t length, unsigned int flags, char id[32], vmm_region_t ** list){
    if(!length)
        return -1;
//	printf("vmm_create_shared_auto(%08x, %08x, %08x, %s)\r\n", base, length, flags, id);
	
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
                // printf("Current region: %08x\r\n", region);
                // Create new region before current one
                vmm_region_t * preregion = kmalloc(sizeof(vmm_region_t));
                preregion->base = region->base;
                preregion->flags = region->flags;
                preregion->length = (size_t)base - (size_t)region->base;
                preregion->next = region;
                preregion->prev = region->prev;
                if(region->prev){
                    region->prev->next = preregion;
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
            region->flags = VMM_FLAGS_USED | flags&(VMM_FLAGS_EXEC|VMM_FLAGS_READ|VMM_FLAGS_WRITE) | VMM_FLAGS_SHARED;
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

			// Create shared block
			region->shared = (vmm_shared_t*)kmalloc(sizeof(vmm_shared_t));
			region->shared->owner = proc_proc_get_current()->id;
			region->shared->next = 0;
			region->shared->phys_base = physical_mem;
			region->shared->phys_length = length;
			memcpy(region->shared->id, id, 32);

			// Link in system wide list
			vmm_shared_t * sr = vmm_shared_regions;
			if(sr){
				while(sr->next) sr = sr->next;
				sr->next = region->shared;
			}else{
				vmm_shared_regions = region->shared;
			}

            // Check if list is prepended
            if((*list)->prev){
                // printf("Set prepend\r\n");
                *list = (*list)->prev;
            }

			// Clean list
			vmm_clean(*list);
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

int vmm_create_shared_auto(void ** base, size_t length, unsigned int flags, char id[32], vmm_region_t ** list){
	if(!length)
		return -1;

//	printf("vmm_create_shared_auto(%08x, %08x, %08x, %s)\r\n", base, length, flags, id);

	vmm_region_t * region = *list;
	while(region){

		// Check if region is unused and has enough size
		if(region->flags==0 && region->length>=length){
			// A usable region found, use it
			void * regbase = region->base;
			int status = vmm_create_shared(region->base, length, flags, id, list);
			if(!status){
				*base = regbase;
				return 0;
			}
			// An error occured, try next region
		}

		region = region->next;
	}
	// Traversed all regions, no feasable found...
	return -1;
}

int vmm_map_shared(void * base, unsigned int flags, unsigned int owner, char id[32], vmm_region_t ** list){

	// Find shared region of owner
	vmm_shared_t * sr = vmm_shared_regions;
	size_t length = 0;
	void * phys = 0;
	while(sr){
		if(sr->owner == owner && strcmp(id, sr->id)==0){
			// Found shared region
			length = sr->phys_length;
			phys = sr->phys_base;
			break;
		}
		sr = sr->next;
	}

	if(sr==0 || length==0){
		return -1;
	}

    // Find the descriptor of the region
    vmm_region_t * region = *list;
    while(region){
        if((size_t)region->base<=(size_t)base && (size_t)region->base+region->length>(size_t)base){
            // Within this current region
            // Check if free
            if((region->flags&VMM_FLAGS_USED)!=0){
                // Already in use
                return -1;
            }
            // Check if within region bounds (if request is not to large)
            if((size_t)region->base+region->length<(size_t)base+length){
                // Request to large
                return -1;
            }
            // Region usable
            // Check if new region must be made before new one
            if((size_t)base>(size_t)region->base){
                // Create new region before current one
                vmm_region_t * preregion = kmalloc(sizeof(vmm_region_t));
                preregion->base = region->base;
                preregion->flags = region->flags;
                preregion->length = (size_t)base - (size_t)region->base;
                preregion->next = region;
                preregion->prev = region->prev;
                if(region->prev){
                    region->prev->next = preregion;
                }
                region->prev = preregion;
                region->length -= preregion->length;
                region->base = base;
            }
            // Check if new region must be made after new one
            if(length<region->length){
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
            }else{
                region->next = 0;
            }
            
            // Allocate new region
            // Set flags
            region->flags = VMM_FLAGS_USED | flags&(VMM_FLAGS_EXEC|VMM_FLAGS_READ|VMM_FLAGS_WRITE) | VMM_FLAGS_SHARED ;
            // Map the physical memory
            unsigned int vas_flags = 0;
            if(flags&VMM_FLAGS_READ)
                vas_flags |= VAS_FLAGS_READ;
            if(flags&VMM_FLAGS_WRITE)
                vas_flags |= VAS_FLAGS_WRITE;
            for(int i=0; i<length/PAGE_SIZE; i++){
                vas_map(phys+i*PAGE_SIZE, base + i*PAGE_SIZE, vas_flags);
            }

			// Point shared region field to shared region descriptor
			region->shared = sr;

            // Check if list is prepended
            if((*list)->prev){
                *list = (*list)->prev;
            }

			// Clean list
			vmm_clean(*list);
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

int vmm_map_shared_auto(void ** base, unsigned int flags, unsigned int owner, char id[32], vmm_region_t ** list){

	// Find shared region of owner
	vmm_shared_t * sr = vmm_shared_regions;
	size_t length = 0;
	void * phys = 0;
	while(sr){
		if(sr->owner == owner && strcmp(id, sr->id)==0){
			// Found shared region
			length = sr->phys_length;
			phys = sr->phys_base;
			break;
		}
		sr = sr->next;
	}

	if(sr==0 || length==0){
		return -1;
	}

	vmm_region_t * region = *list;
	while(region){

		// Check if region is unused and has enough size
		if(region->flags==0 && region->length>=length){
			// A usable region found, use it
			void * regbase = region->base;
			int status = vmm_map_shared(region->base, flags, owner, id, list);
			if(!status){
				*base = regbase;
				return 0;
			}
			// An error occured, try next region
		}

		region = region->next;
	}
	// Traversed all regions, no feasable found...
	return -1;
}


