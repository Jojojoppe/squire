#include <i386/proc.h>
#include <general/kmalloc.h>
#include <general/vmm.h>
#include <i386/memory/vas.h>
#include <general/arch/pmm.h>
#include <general/schedule.h>
#include <general/string.h>
#include <general/elf.h>

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
    proc_proc_current->killed_threads = 0;
    proc_thread_current->next = 0;
    proc_thread_current->prev = 0;
    proc_thread_current->state = PROC_TRHEAD_STATE_RUNNING;
    proc_thread_current->id = 2;
    proc_thread_current->stack = 0;
    proc_thread_current->stack_length = 0;  // Stack is embedded in kernel, cant be freed
    // Set architecture specific data
    proc_thread_arch_data_t * t_arch_data = (proc_thread_arch_data_t*)proc_thread_current->arch_data;
    extern unsigned int * boot_stack_top_C;
    t_arch_data->kstack = proc_create_return_stack_frame(boot_stack_top_C, return_addr, 1, 2, 3, 4, 5, 6);
    t_arch_data->tss_esp0 = 0;
    //printf("kstack = %08x\r\n", t_arch_data->kstack);

    // Initialize message structure of process
    message_init_info(&proc_proc_current->message_info);

    // Switch to created process
    proc_thread_switch(proc_thread_current, 0);

    return 0;
}

void proc_thread_start(){
    void (*return_addr)();
    unsigned int ebx, ecx;
    __asm__ __volatile__("movl %%ebx, %%eax":"=a"(ebx));
    __asm__ __volatile__("movl %%ecx, %%eax":"=a"(ecx));
    __asm__ __volatile__("movl 4(%%ebp), %%eax":"=a"(return_addr));

    // printf("proc_thread_start(%08x) ebx = %08x ecx = %08x\r\n", return_addr, ebx, ecx);

    __asm__ __volatile__("movl %%eax, %%ebx"::"a"(ebx));
    __asm__ __volatile__("movl %%eax, %%ecx"::"a"(ecx));
    __asm__ __volatile__("sti");
    schedule();
    return_addr();

    proc_thread_kill(proc_thread_get_current(), proc_proc_get_current(), 0);

    schedule();
    for(;;){
    }
}

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi){
    unsigned int stack_top = (unsigned int)stack;

    // printf("New stack frame: %08x %08x %08x %08x %08x %08x\r\n", eax, ebx, ecx, edx, esi, edi);

    *(stack-1) = retaddr;
    *(stack-2) = proc_thread_start;               // Return address
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

int proc_thread_switch (proc_thread_t * to, proc_thread_t * from){
    if(to==from)
        return 1;

    extern unsigned int * TSS;

    // printf("* SWITCH %08x -> %08x\r\n", from, to);
    // printf("    from kstack %08x\r\n", TSS[1]);
    // printf("    to kstack %08x\r\n", ((proc_thread_arch_data_t*)to->arch_data)->tss_esp0);

    __asm__ __volatile__("cli");
    proc_thread_current = to;
    if(from){
        // Save current state in from
        __asm__ __volatile__("fsave (%%eax)"::"a"(((proc_thread_arch_data_t*)(from->arch_data))->fpudata));
        __asm__ __volatile__(".intel_syntax noprefix");
        __asm__ __volatile__("pushad");
        __asm__ __volatile__("pushfd");
        __asm__ __volatile__(".att_syntax noprefix");
        __asm__ __volatile__("mov %esp, %esi");
        __asm__ __volatile__("nop":"=S"(((proc_thread_arch_data_t*)from->arch_data)->kstack));
        ((proc_thread_arch_data_t*)from->arch_data)->tss_esp0 = TSS[1];
    }
    TSS[1] = ((proc_thread_arch_data_t*)to->arch_data)->tss_esp0;
    __asm__ __volatile__("frstor (%%eax)"::"a"(((proc_thread_arch_data_t*)(to->arch_data))->fpudata));
    __asm__ __volatile__("nop"::"S"(((proc_thread_arch_data_t*)to->arch_data)->kstack));
    __asm__ __volatile__("mov %esi, %esp");
    __asm__ __volatile__(".intel_syntax noprefix");
    __asm__ __volatile__("popfd");
    __asm__ __volatile__("popad");
    __asm__ __volatile__(".att_syntax noprefix");

    return 0;
}

int proc_proc_switch(proc_proc_t * to, proc_proc_t * from){

    if(!to->threads){
        return 1;
    }

    // Set VAS
    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*)to->arch_data;
    __asm__ __volatile__("mov %%eax, %%cr3"::"a"(arch_data->cr3));

    proc_proc_current = to;
    // proc_thread_switch(to->threads, proc_thread_current);
    return 0;
}

/* Paramters:
 *      edi:    Address to jump to
 *      edx:    User stack address
 *      ecx:    argv data as pairs of [number of bytes, data]
 *      ebx:    argc
 */
void proc_user_exec(){
    __asm__ __volatile__("cli");

    // Get parameters
    unsigned int address, user_stack, argc;
    unsigned int * argv;
    __asm__ __volatile__("nop":"=c"(address));
    __asm__ __volatile__("nop":"=b"(user_stack));
    // __asm__ __volatile__("nop":"=c"(argc));
    // __asm__ __volatile__("nop":"=b"(argv));

    // printf("user exec @ %08x stack: %08x\r\n", address, user_stack);

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

proc_thread_t * proc_thread_get_current(){
    return proc_thread_current;
}

proc_proc_t * proc_proc_get_current(){
    return proc_proc_current;
}

proc_thread_t * proc_thread_new(void * code, void * stack, size_t stack_length, proc_proc_t * process){
    __asm__ __volatile__("cli");
    proc_thread_t * thread = (proc_thread_t*)kmalloc(sizeof(proc_thread_t));
    // Link stucture to process
    proc_thread_t * last = process->threads;
    while(last->next)
        last = last->next;
    last->next = thread;
    thread->prev = last;
    thread->next = 0;
    thread->state = PROC_TRHEAD_STATE_RUNNING;
    thread->stack = stack;
    thread->stack_length = stack_length;
    // Fill other data
    thread->id = proc_PID_counter++;
    proc_thread_arch_data_t * archdata = (proc_thread_arch_data_t*) thread->arch_data;
    archdata->tss_esp0 = 0;
    archdata->kstack = proc_create_return_stack_frame(stack+stack_length-4, code, 0, 0, 0, 0, 0, 0);
    __asm__ __volatile__("sti");
    return thread;
}

proc_thread_t * proc_thread_new_user(void * code, void * stack, size_t stack_length, proc_proc_t * process){
    __asm__ __volatile__("cli");
    proc_thread_t * thread = (proc_thread_t*)kmalloc(sizeof(proc_thread_t));
    // Link stucture to process
    proc_thread_t * last = process->threads;
    if(last){
        while(last->next){
            last = last->next;
        }
        last->next = thread;
        thread->prev = last;
    }else{
        process->threads = thread;
        thread->prev = 0;
    }
    thread->next = 0;
    thread->state = PROC_TRHEAD_STATE_RUNNING;
    // Create kernel stack 
    // TODO fix stack stuff
    void * kstack = vas_brk(4096);
    thread->stack = stack;
    thread->stack_length = stack_length;
    // Fill other data
    thread->id = proc_PID_counter++;
    proc_thread_arch_data_t * archdata = (proc_thread_arch_data_t*) thread->arch_data;
    archdata->tss_esp0 = kstack+4096-4;

    // printf("proc_thread_new_user(%08x, %08x, %08x) [%08x]\r\n", code, stack, stack_length, thread);
    // printf("    user stack %08x\r\n", stack+stack_length-4);
    // printf("    kernel stack %08x\r\n", kstack + 4096 - 4);
    // printf("    last %08x\r\n", last);
    // printf("    last->next %08x\r\n", last->next);

    archdata->kstack = proc_create_return_stack_frame(kstack+4096-4, proc_user_exec, 0, stack+stack_length-4, code, 0, 0, 0);

    __asm__ __volatile__("sti");
    return thread;
}

vmm_region_t * proc_get_memory(){
    return proc_proc_get_current()->memory;
}

void proc_set_memory(vmm_region_t * region){
    proc_proc_get_current()->memory = region;
}

proc_proc_t * proc_proc_new(void * ELF_start){
    schedule_disable();

    // Create new process descriptor
    proc_proc_t * pnew = (proc_proc_t*)kmalloc(sizeof(proc_proc_t));
    proc_proc_t * pcur = proc_proc_get_current();
    // Link new process in list after current
    pnew->next = pcur->next;
    pnew->prev = pcur;
    pcur->next->prev = pnew;
    pcur->next = pnew;
    // Set data
    pnew->id = proc_PID_counter++;
    // Create vmm memory region list
    vmm_create(&pnew->memory);

    // Create new PD table and copy kernel space into it (use page 0 as tmp)
    void * newpd_phys;
    pmm_alloc(4096, &newpd_phys);
    vas_map(newpd_phys, 0, VAS_FLAGS_WRITE|VAS_FLAGS_READ);
    // Copy kernel PD into new one
    memcpy(768*4, 0xfffff000+768*4, 255*4);
    *((unsigned int*)(768*4+255*4)) = (unsigned int)newpd_phys | 0x07;
    // Clear first part to be sure
    memset(0, 0, 256*4*3);
    vas_unmap(0);
    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*) pnew->arch_data;
    arch_data->cr3 = newpd_phys;


    // switch to new VAS
    unsigned int own_vas = ((proc_proc_arch_data_t*)(pcur->arch_data))->cr3;
    proc_proc_current = pnew;
    __asm__ __volatile__("movl %%eax, %%cr3"::"a"(newpd_phys));

    // Load ELF
    void (*entry)();
    elf_load_simple(ELF_start, &entry);

    // Create new thread
    // printf("Create new user stack\r\n");
    vmm_region_t * proc_mem = proc_get_memory();
    vmm_alloc(0xbfffc000,0x4000,VMM_FLAGS_READ|VMM_FLAGS_WRITE,&proc_mem);
    // printf("Create new user thread\r\n");
    pnew->threads = 0;
    pnew->killed_threads = 0;
    proc_thread_new_user(entry, 0xbfffc000, 0x4000, pnew);
    // printf("Done\r\n");
    proc_set_memory(proc_mem);

    // switch back to old VAS
    proc_proc_current = pcur;
    __asm__ __volatile__("movl %%eax, %%cr3"::"a"(own_vas));

    // TODO switch to new VAS, load elf, setup user stack, add thread discriptor
    // and go back to own VAS
    // Setup messaging info
    message_init_info(&pnew->message_info);
    // printf("messages initialized\r\n");

    // pnew->threads->state = PROC_THREAD_STATE_WAITING;

    schedule_enable();
    return pnew;
}

int proc_thread_kill(proc_thread_t * thread, proc_proc_t * process, int retval){

    // Check if thread is in process
    proc_thread_t * t = process->threads;
    while(t){
        if(thread==t){
            schedule_disable();

            // Unlink thread from thread list
            if(process->threads==thread){
                // Set next as first if needed
                process->threads = thread->next;
            }else{
                // Set previous
                thread->prev->next = thread->next;
            }
            if(thread->next){
                thread->next->prev = thread->prev;
            }

            // Relink to killed threads list
            // Find last in killed threads
            proc_thread_t * kt = process->killed_threads;
            thread->next = 0;
            if(kt==0){
                process->killed_threads = thread;
                thread->prev = 0;
            }else{
                while(kt->next){
                    kt = kt->next;
                }
                kt->next = thread;
                thread->prev = kt;
            }
            
            thread->retval = retval;

            // TODO if all threads are killed: set process retval as last thread retval and
            // kill process itself

            schedule_enable();
            return 0;
        }
        t = t->next;
    }
    // Thread not in process: ERROR
    return 1;
}

void proc_debug(){
    proc_proc_t * p = proc_proc_get_current();
    proc_proc_t * current = p;
    printf("Processes:\r\n");
    do{
        printf("+ P[%d] - %08x\r\n", p->id, p);
        printf("  next: %08x\r\n", p->next);
        printf("  prev: %08x\r\n", p->prev);
        proc_thread_t * t = p->threads;
        while(t){
            printf("  - T[%d] /w %08x (%08x) \r\n", t->id, t->stack, t->stack_length);
            t = t->next;
        }
        t = p->killed_threads;
        while(t){
            printf("  - KT[%d] -> %08x\r\n", t->id, t->retval);
            t = t->next;
        }
        p = p->next;
    }while(p!=current);
}

proc_proc_t * proc_get(unsigned int pid){
    proc_proc_t * proc = 0;

    proc_proc_t * p = proc_proc_get_current();
    proc_proc_t * current = p;
    do{
        if(p->id==pid){
            proc = p;
            break;
        }
        p = p->next;
    }while(p!=current);

    return proc;
}


proc_thread_t * proc_thread_get(unsigned int tid, unsigned int pid){
    proc_proc_t * p = proc_proc_get_current();
    proc_proc_t * current = p;
    do{
        proc_thread_t * t = p->threads;
        while(t){
            if(t->id == tid && p->id == pid){
                return t;
            }
            t = t->next;
        }
        p = p->next;
    }while(p!=current);
    return 0;
}

proc_proc_t * proc_proc_fork(){
    schedule_disable();

    // Create new process descriptor
    proc_proc_t * pnew = (proc_proc_t*)kmalloc(sizeof(proc_proc_t));
    proc_proc_t * pcur = proc_proc_get_current();
    // Link new process in list after current
    pnew->next = pcur->next;
    pnew->prev = pcur;
    pcur->next->prev = pnew;
    pcur->next = pnew;
    // Set data
    pnew->id = proc_PID_counter++;
    // Create vmm memory region list
    vmm_create(&pnew->memory);

    // Create new PD table and copy kernel space into it (use page 0 as tmp)
    void * newpd_phys;
    pmm_alloc(4096, &newpd_phys);
    vas_map(newpd_phys, 0, VAS_FLAGS_WRITE|VAS_FLAGS_READ);
    // Copy kernel PD into new one
    memcpy(768*4, 0xfffff000+768*4, 255*4);
    *((unsigned int*)(768*4+255*4)) = (unsigned int)newpd_phys | 0x07;
    // Clear first part to be sure
    memset(0, 0, 256*4*3);
    vas_unmap(0);
    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*) pnew->arch_data;
    arch_data->cr3 = newpd_phys;

    // Copy memory to new process
    vmm_region_t * region = pcur->memory;
    while(region){
        printf("region [%08x]:\n", region);
        printf(" - base:    %08x\n", region->base);
        printf(" - length:  %08x\n", region->length);
        printf(" - flags:   %08x\n", region->flags);
        printf(" - next:    %08x\r\n", region->next);

        // switch to new VAS
        unsigned int own_vas = ((proc_proc_arch_data_t*)(pcur->arch_data))->cr3;
        proc_proc_current = pnew;
        __asm__ __volatile__("movl %%eax, %%cr3"::"a"(newpd_phys));

        // Allocate region in new VAS
        vmm_alloc(region->base, region->length, region->flags, &pnew->memory);

        // switch back to old VAS
        proc_proc_current = pcur;
        __asm__ __volatile__("movl %%eax, %%cr3"::"a"(own_vas));

        for(int i=0; i<region->length/4096; i++){
            // Get physical page of page to copy
            unsigned phys = vas_get_pte(region->base+4096*i)&0xfffff000;

            // switch to new VAS
            unsigned int own_vas = ((proc_proc_arch_data_t*)(pcur->arch_data))->cr3;
            proc_proc_current = pnew;
            __asm__ __volatile__("movl %%eax, %%cr3"::"a"(newpd_phys));

            // Map physical to temporary page here
            vas_map(phys, 0, VAS_FLAGS_READ);

            // Copy data to new page
            memcpy(region->base+i*4096, 0, 4096);

            // Unmap old physical
            vas_unmap(0);

            // switch back to old VAS
            proc_proc_current = pcur;
            __asm__ __volatile__("movl %%eax, %%cr3"::"a"(own_vas));
        }

        // To next region
        if(region->next && region->next!=region){
            region = region->next;
            continue;
        }
        // No more regions
        break;
    }   

    // Copy threads to new process
    proc_thread_t * curt = pcur->threads;
    while(curt){
        curt = curt->next;
    }

    schedule_enable();
    return pnew;
}