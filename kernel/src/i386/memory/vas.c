#include <i386/memory/vas.h>
#include <i386/memory/pmm.h>

#define KERNEL_PT 0xffc00000
#define KERNEL_PD 0xfffff000

static unsigned int vas_k_brk;

int vas_init(){
    // Set kernel break
    vas_k_brk = 0xc0000000 + 4*1024*1024;
    return 0;
}

int vas_map(void * physical, void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry, create page table
        void * newpt_physical;
        pmm_alloc(4096, &newpt_physical);
        PDE = *((unsigned int*)(KERNEL_PD)+PD) = (unsigned int)newpt_physical | 0x07;
    }
    // Set PTE
    *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | 0x07;
    return 0;
}

int vas_unmap(void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry, create page table
        void * newpt_physical;
        pmm_alloc(4096, &newpt_physical);
        PDE = *((unsigned int*)(KERNEL_PD)+PD) = (unsigned int)newpt_physical | 0x07;
    }
    // Set PTE
    *((unsigned int*)(KERNEL_PT)+PT) = 0;
    return 0;;
}

void * vas_brk(size_t length){
    void * physical;
    pmm_alloc(length, &physical);
    for(int i=0; i<length/4096; i++){
        vas_map((unsigned int)physical+4096*i, vas_k_brk+4096*i);
    }
    unsigned int old_brk = vas_k_brk;
    vas_k_brk += 4096*(length/4096);
    return (void*)old_brk;
}