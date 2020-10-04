#include <i386/proc.h>
#include <general/kmalloc.h>
#include <general/vmm.h>
#include <i386/memory/vas.h>

extern unsigned int * TSS;

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi);
void proc_thread_start();

int proc_init(void (*return_addr)()){
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
    t_arch_data->kstack = proc_create_return_stack_frame(boot_stack_top_C, return_addr, 1, 2, 3, 4, 5, 6);
    //printf("kstack = %08x\r\n", t_arch_data->kstack);

    // Switch to created process
    proc_thread_switch(proc_thread_current, 0);

    return 0;
}

void proc_thread_start(){
    void (*return_addr)();
    __asm__ __volatile__("movl 4(%%ebp), %%eax":"=a"(return_addr));

    return_addr();

    for(;;);
}

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi){
    unsigned int stack_top = (unsigned int)stack;

    *(stack-2) = retaddr;               // Return address
    *(stack-3) = proc_thread_start;     // Return address of proc_thread_start
    *(stack-4) = stack_top;             // Old ebp
    *(stack-5) = eax;
    *(stack-6) = ecx;
    *(stack-7) = edx;
    *(stack-8) = ebx;
    *(stack-9) = stack_top-12;          // esp (=ebp)
    *(stack-10) = stack_top-12;          // ebp
    *(stack-11) = esi;
    *(stack-12) = edi;
    *(stack-13) = 0x00200086;           // EFLAGS

    return (void*)(stack-13);
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
    //asm("int $2");

    return 0;
}

/* Paramters:
 *      eax:    Address to jump to
 *      edx:    User stack address
 *      ecx:    argv data as pairs of [number of bytes, data]
 *      ebx:    argc
 */
void proc_user_exec(){
    __asm__ __volatile__("cli");

    // Get parameters
    unsigned int address, user_stack, argc;
    unsigned int * argv;
    __asm__ __volatile__("nop":"=a"(address));
    __asm__ __volatile__("nop":"=d"(user_stack));
    __asm__ __volatile__("nop":"=c"(argc));
    __asm__ __volatile__("nop":"=b"(argv));

    // Load current esp as kernel stack in tss
    unsigned int esp;
    __asm__ __volatile__("mov %%esp, %%eax":"=a"(esp));
    TSS[1] = esp;
    TSS[2] = 0x10;

    // Get EFLAGS
    unsigned int EFLAGS;
    __asm__ __volatile__("pushf");
    __asm__ __volatile__("pop %%eax":"=a"(EFLAGS));
    // Re-enable interrupts
    EFLAGS |= 0x200;

    // Jump to user space
    __asm__ __volatile__("mov $0x23, %eax");
    __asm__ __volatile__("mov %ax, %ds");
    __asm__ __volatile__("mov %ax, %es");
    __asm__ __volatile__("mov %ax, %fs");
    __asm__ __volatile__("mov %ax, %gs");

    __asm__ __volatile__("push %eax");
    __asm__ __volatile__("push %%eax"::"a"(user_stack));
    __asm__ __volatile__("push %%eax"::"a"(EFLAGS));
    __asm__ __volatile__("mov $0x1b, %eax");
    __asm__ __volatile__("push %eax");
    __asm__ __volatile__("push %%eax"::"a"(address));

    // Clear registers
    __asm__ __volatile__("xor %eax, %eax");
    __asm__ __volatile__("mov %eax, %ebx");
    __asm__ __volatile__("mov %eax, %ecx");
    __asm__ __volatile__("mov %eax, %edx");
    __asm__ __volatile__("mov %eax, %esi");
    __asm__ __volatile__("mov %eax, %edi");
    __asm__ __volatile__("mov %%eax, %%ebp"::"a"(user_stack));
    __asm__ __volatile__("xor %eax, %eax"); // Param data in eax

    __asm__ __volatile__("iret");
}