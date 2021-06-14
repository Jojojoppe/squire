#include "vas.h"
#include "../../common/kprintf.h"
#include "pmm.h"

#define KERNEL_PT 0xffc00000
#define KERNEL_PD 0xfffff000

void arch_vas_init(){
}

unsigned int arch_vas_getcr3(){
    unsigned int cr3;
    __asm__ __volatile__("movl %%cr3, %%eax":"=a"(cr3));
    return cr3;
}

unsigned int arch_vas_getpte(void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry
        return 0;
    }
    unsigned int PTE = *((unsigned int*)(KERNEL_PT)+PT);
    return PTE;
}

unsigned int arch_vas_getpde(void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    return PDE;
}

int arch_vas_map(void * physical, void * virtual, unsigned int flags){
    // Get PD and PT
    unsigned int PT = (unsigned int)virtual>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if((PDE&0x01)==0){
        // There is no PD entry, create page table
        void * newpt_physical;
        newpt_physical = arch_pmm_alloc(1, PMM_MEMORY_TYPE_NORMAL);
        PDE = *((unsigned int*)(KERNEL_PD)+PD) = (unsigned int)newpt_physical | 0x07;
    }
    // Set PTE

    unsigned int PTE = (unsigned int)physical | 0x1;

    // Access permissions
    switch(flags&0x6c){
        // Priviledged read only
        case VAS_FLAGS_KREAD:
            break;

        // Read only
        case VAS_FLAGS_KREAD | VAS_FLAGS_UREAD:
            PTE |= 0x6;
            break;

        // Priviledged access only
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE:
        case VAS_FLAGS_KWRITE:
            PTE |= 0x2;
            break;

        // No user-mode write
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD:
            PTE |= 0x6;
            break;

        // Full access
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UREAD | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KREAD | VAS_FLAGS_KWRITE | VAS_FLAGS_UWRITE:
        case VAS_FLAGS_KWRITE | VAS_FLAGS_UWRITE:
            PTE |= 0x6;
            break;

        default:
            // Unknown combination of flags
            return 1;
    }

    *((unsigned int*)(KERNEL_PT)+PT) = PTE;
    
    __asm__ __volatile__("movl %cr3,%eax; movl %eax,%cr3");
    return 0;
}

int arch_vas_unmap(void * virtual){
    // Get PD and PT
    unsigned int PT = (unsigned int)virtual>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry
        return 1;
    }
    // Set PTE
    *((unsigned int*)(KERNEL_PT)+PT) = 0;
    return 0;
}