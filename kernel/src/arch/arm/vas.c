#include "vas.h"
#include "pmm.h"

// Current Page directory
unsigned int * arch_vas_pd;
unsigned int * arch_vas_tmppt;

#define invalidate_tlb(){\
         __asm__  __volatile__ (\
            "mcr p15, 0, r0, c8, c3, 0" \
        );\
    }

void arch_vas_init(){
    // Use directly mapped PD in kernel
    extern unsigned int * boot_pd_c;
    arch_vas_pd = boot_pd_c;

    // Total PD is 16K (4 frames that is 4 page table entries)
    // Create page table for top 1MB
    void * newpt = arch_pmm_alloc(1, PMM_MEMORY_TYPE_FAST);
    // Map new page table and surrounding 1MB to first 1MB of system
    arch_vas_pd[0] = (unsigned int)newpt&0xfff00000 | 0x402;
    // Get offset in 1MB
    unsigned int * newpt_virt = (unsigned int*)((unsigned int)newpt & 0x000fffff);

    // Map table itself to first entry (ffe00000)
    newpt_virt[0] = (unsigned int)newpt&0xfffff000 | 0x013;

    // Map page directory to top 4 entries
    int j=0;
    for(int i=252; i<256; i++){
        newpt_virt[i] = ((unsigned int)boot_pd_c+4096*j-0xc0000000)&0xfffff000 | 0x013;
        j++;
    }

    // Use new page table
    arch_vas_pd[0xfff] = (unsigned int)newpt&0xfffffc00 | 0x001;
    // Unmap first 1MB
    arch_vas_pd[0] = 0;
    // Set PD to top 16K
    arch_vas_pd = (unsigned int*)0xffffc000;
    arch_vas_tmppt = (unsigned int*)0xfff00000;
    invalidate_tlb();
}

void * arch_vas_create_pt(){
    // FIXME place 4 page tables in a frame
    return arch_pmm_alloc(1, PMM_MEMORY_TYPE_FAST);
}

int arch_vas_map(void * physical, void * virtual, unsigned int flags){
    unsigned int PT = ((unsigned int)virtual>>12)&0xff;
    unsigned int PD = (unsigned int)virtual>>20;
    unsigned int PDE = *((unsigned int*)(arch_vas_pd)+PD);

    if((PDE&0x3)>=2){
        // Reserved or (super)section, cannot map in this region
        return 1;
    }

    unsigned int * table = (unsigned int*)(PDE&0xfffffc00);
    if((PDE&0x3)==0){
        // Need to create new table and update PD
        table = arch_vas_create_pt();
        // Create new PD entry
        PDE = (((unsigned int)table)&0xfffffc00) | 0x1;
        arch_vas_pd[PD] = PDE;
    }

    // Set page attributes
    // TODO cache??
    unsigned int PTE = 0x002;

    // Access permissions
    switch(flags&0x6c){
        // Priviledged read only -> AP 101
        case VAS_FLAGS_KREAD:
            PTE |= 0x210;
            break;

        // Read only -> AP 110
        case VAS_FLAGS_KREAD | VAS_FLAGS_UREAD:
            PTE |= 0x220;
            break;

        // Priviledged access only -> AP 001
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE:
        case VAS_FLAGS_KWRITE:
            PTE |= 0x010;
            break;

        // No user-mode write -> AP 010
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD:
            PTE |= 0x020;
            break;

        // Full access -> 011
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UWRITE:
            PTE |= 0x030;
            break;

        default:
            // Unknown combination of flags
            return 1;
    }

    // Check for execute
    if(((flags&VAS_FLAGS_KEXEC)==0)&&((flags&VAS_FLAGS_UEXEC)==0)){
        // Never executable
        PTE |= 0x1;
    }

    // Add physical address
    PTE |= (unsigned int)physical & 0xfffff000;

    // Map page table into workable memory
    unsigned int PT_addr = arch_vas_pd[PD]&0xfffffc00;
    arch_vas_tmppt[1] = PT_addr | 0x013;

    unsigned int * newpt = (unsigned int*) 0xfff01000;
    newpt[PT] = PTE;

    // Unmap page table from workable memory
    arch_vas_tmppt[1] = 0;
    return 0;
}

int arch_vas_unmap(void * virtual){

    return 0;
}