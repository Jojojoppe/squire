#include <i386/proc.h>
#include <general/kmalloc.h>
#include <general/vmm.h>
#include <i386/memory/vas.h>

extern unsigned int * TSS;

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi);
void proc_thread_start();

int proc_init(){
    void * return_addr;
    __asm__ __volatile__("movl 4(%%ebp), %%eax":"=a"(return_addr));

    // Initialize the TSS
    TSS[1] = 0 ;        // ESP0
    TSS[2] = 0x10;      // SS0
    TSS[25] = 104;      // IOPB

    proc_PID_counter = 3;

    // Create main process structure
    proc_proc_current = (proc_proc_t*)kmalloc(sizeof(proc_proc_t));
    proc_proc_current->next = proc_proc_current;
    proc_proc_current->prev = proc_proc_current;
    proc_proc_current->id = 1;
    // Create memory region
    vmm_create(&proc_proc_current->memory);
    // Set architecture specific data
    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*)proc_proc_current->arch_data;
    arch_data->cr3 = vas_getcr3();

    // Create first thread structure
    proc_thread_current = (proc_thread_t*)kmalloc(sizeof(proc_thread_t));
    proc_proc_current->threads = proc_thread_current;
    proc_thread_current->next = 0;
    proc_thread_current->prev = 0;
    proc_thread_current->id = 2;
    // Set architecture specific data
    proc_thread_arch_data_t * t_arch_data = (proc_thread_arch_data_t*)proc_thread_current->arch_data;
    t_arch_data->tss_esp0 = 0;
    extern unsigned int * boot_stack_top_C;
    t_arch_data->kstack = proc_create_return_stack_frame(boot_stack_top_C, return_addr, 1, 2, 3, 4, 5, 6)-8;
    printf("kstack = %08x\r\n", t_arch_data->kstack);

    // Switch to created process
    proc_thread_switch(proc_thread_current, 0);

    return 0;
}

void proc_thread_start(){
    printf("PROC_THREAD_START\r\n");
    __asm__ __volatile__("int $0");
    __asm__ __volatile__("sti");
    __asm__ __volatile__("ret");
}

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi){
    unsigned int stack_top = (unsigned int)stack;
    *(--stack) = retaddr;
    *(--stack) = stack_top;
    *(--stack) = proc_thread_start;
    *(--stack) = stack_top;
    *(--stack) = eax;
    *(--stack) = ecx;
    *(--stack) = edx;
    *(--stack) = ebx;
    *(--stack) = 0;
    *(--stack) = stack_top-16;
    *(--stack) = esi;
    *(--stack) = edi;
    *(--stack) = 0x00200202;

    return (void*)(stack);
}

int proc_thread_switch(proc_thread_t * to, proc_proc_t * from){
    __asm__ __volatile__("cli");
    extern unsigned int * TSS;
    proc_thread_current = to;
    if(from){
        // Save current state in from
        __asm__ __volatile__("fsave (%%eax)"::"a"(((proc_thread_arch_data_t*)(from->arch_data))->fpudata));
        __asm__ __volatile__(".intel_syntax noprefix");
        __asm__ __volatile__("pushad");
        __asm__ __volatile__("pushfd");
        __asm__ __volatile__(".att_syntax noprefix");
        __asm__ __volatile__("mov %%esp, %%eax":"=a"(((proc_thread_arch_data_t*)from->arch_data)->kstack));
        ((proc_thread_arch_data_t*)from->arch_data)->tss_esp0 = TSS[4];
    }
    TSS[4] = ((proc_thread_arch_data_t*)to->arch_data)->tss_esp0;
    __asm__ __volatile__("frstor (%%eax)"::"a"(((proc_thread_arch_data_t*)(to->arch_data))->fpudata));
    __asm__ __volatile__("mov %%eax, %%esp"::"a"(((proc_thread_arch_data_t*)to->arch_data)->kstack));
    __asm__ __volatile__(".intel_syntax noprefix");
    __asm__ __volatile__("popfd");
    __asm__ __volatile__("popad");
    __asm__ __volatile__(".att_syntax noprefix");
    asm("int $0");

    return 0;
}