#include <i386/memory/vas.h>
#include <i386/memory/pmm.h>

#define KERNEL_PT 0xffc00000
#define KERNEL_PD 0xfffff000

#define COW_BIT 9
#define AOA_BIT 10

static unsigned int vas_k_brk;

int vas_init(){
    // Set kernel break
    vas_k_brk = 0xc0000000 + 4*1024*1024;
    return 0;
}

int vas_map(void * physical, void * address, unsigned int flags){
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
    if((flags&VAS_FLAGS_WRITE)!=0){
        *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | 0x07;
    }else{
        *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | 0x05;
    }

    // Check for AOA
    if((flags&VAS_FLAGS_AOA)!=0){
        *((unsigned int*)(KERNEL_PT)+PT) &= 0x00000ffe; // Mark unused
        *((unsigned int*)(KERNEL_PT)+PT) |= 1<<AOA_BIT;
    }
    
    asm("movl %cr3,%eax; movl %eax,%cr3");
    return 0;
}

int vas_unmap(void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry
        return 1;
    }
    // Set PTE
    *((unsigned int*)(KERNEL_PT)+PT) = 0;
    asm("movl %cr3,%eax; movl %eax,%cr3");
    return 0;
}

int vas_unmap_free(void * address){
    // Get PD and PT
    unsigned int PT = (unsigned int)address>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry
        return 1;
    }
    // If there is a physical page: free
    if((*((unsigned int*)(KERNEL_PT)+PT)&1)==1){
        pmm_free(4096, *((unsigned int*)(KERNEL_PT)+PT) & 0xfffff000);
    }
    // Set PTE
    *((unsigned int*)(KERNEL_PT)+PT) = 0;
    asm("movl %cr3,%eax; movl %eax,%cr3");
    return 0;;
}

void * vas_brk(size_t length){
    void * physical;
    pmm_alloc(length, &physical);
    for(int i=0; i<length/4096; i++){
        vas_map((unsigned int)physical+4096*i, vas_k_brk+4096*i, VAS_FLAGS_READ|VAS_FLAGS_WRITE|VAS_FLAGS_AOA);
    }
    unsigned int old_brk = vas_k_brk;
    vas_k_brk += 4096*(length/4096);
    return (void*)old_brk;
}

unsigned int vas_getcr3(){
    unsigned int cr3;
    __asm__ __volatile__("movl %%""cr3, %%eax":"=a"(cr3));
    return cr3;
}

unsigned int vas_get_pte(void * addr){
    // Get PD and PT
    unsigned int PT = (unsigned int)addr>>12;
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

int vas_pagefault(void * addr, unsigned int error){
    // Get PD and PT
    unsigned int PT = (unsigned int)addr>>12;
    unsigned int PD = PT>>10;
    // Get PDE
    unsigned int PDE = *((unsigned int*)(KERNEL_PD)+PD);
    if(!PDE&0x01){
        // There is no PD entry
        return 1;
    }
    unsigned int PTE = *((unsigned int*)(KERNEL_PT)+PT);

    unsigned int P = (error>>0)&1;
    unsigned int RW = (error>>1)&1;
    unsigned int US = (error>>2)&1;

    // Check for AOA
    if(P==0 && PTE&(1<<AOA_BIT)){
        // Allocate space now
        void * physical;
        pmm_alloc(4096, &physical);
        *((unsigned int*)(KERNEL_PT)+PT) = (unsigned int)physical | (PTE&0xfff) | 1;
        *((unsigned int*)(KERNEL_PT)+PT) &= ~(1<<AOA_BIT);

        unsigned int PTE = *((unsigned int*)(KERNEL_PT)+PT);
        
        return 0;
    }

    printf("\r\nPAGE FAULT\r\n----------\r\nP  [%d]\tRW [%d]\tUS [%d]\r\n", P, RW, US);
    printf("COW[%d]\tAOA[%d]\r\n", (PTE>>COW_BIT)&1, (PTE>>AOA_BIT)&1);
    printf("----------");

    return 1;
}