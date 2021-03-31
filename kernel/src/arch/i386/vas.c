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
    if((flags&VAS_FLAGS_WRITE)!=0){
        *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | 0x07;
    }else{
        *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | 0x05;
    }
    
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