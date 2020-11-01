#include <i386/proc.h>
#include <general/kmalloc.h>
#include <general/vmm.h>
#include <i386/memory/vas.h>
#include <general/arch/pmm.h>
#include <general/schedule.h>
#include <general/string.h>
#include <general/elf.h>
#include <general/kill.h>

extern unsigned int * TSS;

#define KERNEL_STACK_TOP 0xffc00000
#define KERNEL_STACK_SIZE 4096

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi);
void proc_thread_start();

extern unsigned int boot_stack_top_C;

int proc_init(void (*return_addr)()){
    // Initialize the TSS
    TSS[1] = 0 ;        // ESP0
    TSS[2] = 0x10;      // SS0
    TSS[25] = 104<<16;  // IOPB

    proc_PID_counter = 2;

    // Create main process structure
    proc_proc_current = (proc_proc_t*)kmalloc(sizeof(proc_proc_t));
    proc_proc_current->id = 1;
    proc_proc_current->kernel_stacks = 1;
    proc_proc_current->tid_counter = 2;
	proc_proc_current->mutexes = 0;

    // Create memory region
    vmm_create(&proc_proc_current->memory);
    // Set architecture specific data
    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*)proc_proc_current->arch_data;
    arch_data->cr3 = vas_getcr3();

    // Allocate first thread kernel stack
    for(int i=0; i<KERNEL_STACK_SIZE/4096; i++){
        // printf("map %08x\r\n", KERNEL_STACK_TOP-(i+1)*4096);
        vas_map(0, KERNEL_STACK_TOP-(i+1)*4096, VAS_FLAGS_READ|VAS_FLAGS_WRITE|VAS_FLAGS_AOA);
    }

    // Create first thread structure
    proc_thread_current = (proc_thread_t*)kmalloc(sizeof(proc_thread_t));
    proc_proc_current->threads = proc_thread_current;
    proc_proc_current->killed_threads = 0;
    proc_thread_current->next = 0;
    proc_thread_current->prev = 0;
    proc_thread_current->id = 1;
    proc_thread_current->stack = 0;
    proc_thread_current->stack_length = 0; // No user stack
    proc_thread_current->kernel_stack = KERNEL_STACK_TOP-KERNEL_STACK_SIZE;
    proc_thread_current->kernel_stack_length = KERNEL_STACK_SIZE;
    // Set architecture specific data
    proc_thread_arch_data_t * t_arch_data = (proc_thread_arch_data_t*)proc_thread_current->arch_data;
    t_arch_data->kstack = proc_create_return_stack_frame(KERNEL_STACK_TOP-4, return_addr, 1, 2, 3, 4, 5, 6);
    t_arch_data->tss_esp0 = 0;
	// Set FPU things
	unsigned short * fpudata = t_arch_data->fpudata;
	fpudata[0] = 0x037f;
	fpudata[2] = 0x0000;

    // Initialize message structure of process
    message_init_info(&proc_proc_current->message_info);

    proc_proc_current->parent = 0;
    proc_proc_current->childs = 0;
    proc_proc_current->child_next = 0;
    proc_proc_current->threads_number = 1;

    proc_proc_current->signal_handler = 0;
    proc_proc_current->signals = 0;

    // Switch to created process
    schedule_init(proc_proc_current, proc_thread_current);
    proc_switch(proc_thread_current, 0, proc_proc_current, 0);

    return 0;
}

void proc_thread_start(){
    __asm__ __volatile__("mov 4(%ebp), %eax; sti; call *%eax"); //":"=a"(eax));
    schedule_kill(0, 0);
    schedule();
    for(;;){
    }
}

void * proc_create_return_stack_frame(unsigned int * stack, void * retaddr, unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi){
    unsigned int stack_top = (unsigned int)stack;

    // printf("New stack frame [%08x]: %08x %08x %08x %08x %08x %08x\r\n", stack, eax, ebx, ecx, edx, esi, edi);

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

void proc_switch(proc_thread_t * tto, proc_thread_t * tfrom, proc_proc_t * pto, proc_proc_t * pfrom){
    __asm__ __volatile__("cli");

    extern unsigned int * TSS;

    proc_proc_current = pto;
    proc_thread_current = tto;
    unsigned int newcr3 = ((proc_proc_arch_data_t*)&pto->arch_data)->cr3;
    unsigned int oldcr3 = vas_getcr3();
    if(tfrom){
        // Save current state in from
        __asm__ __volatile__("fwait; fsave (%%eax); fwait"::"a"(((proc_thread_arch_data_t*)(tfrom->arch_data))->fpudata));
        __asm__ __volatile__("pusha");
        __asm__ __volatile__("pushf");
        __asm__ __volatile__("movl %esp, %edx");
        __asm__ __volatile__("nop":"=d"(((proc_thread_arch_data_t*)tfrom->arch_data)->kstack));
        ((proc_thread_arch_data_t*)tfrom->arch_data)->tss_esp0 = TSS[1];
    }
    TSS[1] = ((proc_thread_arch_data_t*)tto->arch_data)->tss_esp0;
    __asm__ __volatile__("movl %%eax, %%edi"::"a"(newcr3));
    __asm__ __volatile__("fwait; frstor (%%eax); fwait"::"a"(((proc_thread_arch_data_t*)(tto->arch_data))->fpudata));
    __asm__ __volatile__("nop"::"a"(((proc_thread_arch_data_t*)tto->arch_data)->kstack));
    __asm__ __volatile__("movl %eax, %esp; movl %edi, %cr3");
    __asm__ __volatile__("popf");
    __asm__ __volatile__("popa");
    __asm__ __volatile__("leave; ret");

    return;
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
    __asm__ __volatile__("nop":"=d"(user_stack));

	//printf("proc_user_exec(%08x, %08x)\r\n", address, user_stack);
	//hexDump("First few bytes of executable", address, 64);

    // Load current esp as kernel stack in tss
    unsigned int esp;
    __asm__ __volatile__("mov %%esp, %%eax":"=a"(esp));
    TSS[1] = esp;
    TSS[2] = 0x10;
	TSS[25] = 104<<16;

    // Get EFLAGS
    unsigned int EFLAGS;
    __asm__ __volatile__("pushf");
    __asm__ __volatile__("pop %%eax":"=a"(EFLAGS));
    // Re-enable interrupts
    EFLAGS |= 0x200;
	// Set IOPL=0
	EFLAGS &= ~0x3000;

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
    __asm__ __volatile__("xor %eax, %eax");

    __asm__ __volatile__("iret");
}

proc_thread_t * proc_thread_get_current(){
    return proc_thread_current;
}

proc_proc_t * proc_proc_get_current(){
    return proc_proc_current;
}

proc_thread_t * proc_thread_new_user(void * code, void * stack, size_t stack_length, proc_proc_t * process){
    schedule_disable();

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

    // Create kernel stack 
    void * kstack_base = KERNEL_STACK_TOP - (1+process->kernel_stacks)*KERNEL_STACK_SIZE;
    void * physicals;
    pmm_alloc(KERNEL_STACK_SIZE, &physicals);
    for(int i=0; i<KERNEL_STACK_SIZE/4096; i++){
        vas_map(physicals + i*4096, kstack_base+i*4096, VAS_FLAGS_READ|VAS_FLAGS_WRITE);
    }
    thread->stack = stack;
    thread->stack_length = stack_length;
    thread->kernel_stack = kstack_base;
    thread->kernel_stack_length = KERNEL_STACK_SIZE;
    process->kernel_stacks++;
    process->threads_number++;

    // Fill other data
    thread->id = process->tid_counter++;
    proc_thread_arch_data_t * archdata = (proc_thread_arch_data_t*) thread->arch_data;
    archdata->tss_esp0 = 4; //kstack_base+KERNEL_STACK_SIZE-4;

	// Set FPU things
	unsigned short * fpudata = archdata->fpudata;
	fpudata[0] = 0x037f;
	fpudata[2] = 0x0000;

    archdata->kstack = proc_create_return_stack_frame(kstack_base+KERNEL_STACK_SIZE-4, proc_user_exec, 0, 0, code, stack+stack_length-4, 0, 0);

    schedule_add(proc_proc_current, thread, SCHEDULE_QUEUE_TYPE_NORMAL);

    schedule_enable();
    return thread;
}

vmm_region_t * proc_get_memory(){
    return proc_proc_get_current()->memory;
}

void proc_set_memory(vmm_region_t * region){
    proc_proc_get_current()->memory = region;
}

proc_proc_t * _0_proc_proc_new(void * ELF_start){

    // Create new process descriptor
    proc_proc_t * pnew = (proc_proc_t*)kmalloc(sizeof(proc_proc_t));
    proc_proc_t * pcur = proc_proc_get_current();
    // Set data
    pnew->id = proc_PID_counter++;
    pnew->tid_counter = 1;

    pnew->parent = pcur;
    // Add child to parent list
    pnew->child_next = 0;
    proc_proc_t * pcur_childs = pcur->childs;
    if(!pcur_childs){
        pcur->childs = pnew;
    }else{
        while(pcur_childs->child_next){
            pcur_childs = pcur_childs->child_next;
        }
        pcur_childs->child_next = pnew;
    }

    // Create vmm memory region list
    vmm_create(&pnew->memory);

    // Create new PD table and copy kernel space into it (use page 0 as tmp)
    void * newpd_phys;
    pmm_alloc(4096, &newpd_phys);
    vas_map(newpd_phys, 0, VAS_FLAGS_WRITE|VAS_FLAGS_READ);
    memset(0, 0, 4096);
    // Copy kernel PD into new one
    memcpy(768*4, 0xfffff000+768*4, 255*4);
    *((unsigned int*)(768*4+255*4)) = (unsigned int)newpd_phys | 0x07;

    // Get current kstack pde
    // printf("current kstack pde: %08x\r\n", vas_get_pde(0xffbff000));
    // printf("current kstack pte: %08x\r\n", vas_get_pte(0xffbff000));
    // hexDump("Current PT of stacks", 0xffffe000, 4096);

    unsigned int amount_stack_pagetables = (pcur->kernel_stacks/4096+1);
    for(int i=0; i<amount_stack_pagetables; i++){
        *((unsigned int*)(768*4+255*4-4-4*i)) = 0;
        // printf("Clear one stack page for kstack\r\n");
    }

    proc_proc_arch_data_t * arch_data = (proc_proc_arch_data_t*) pnew->arch_data;
    arch_data->cr3 = newpd_phys;

    // hexDump("\r\nThe new PD\r\n", 0, 4096);

    vas_unmap(0);

    // switch to new VAS
    unsigned int own_vas = ((proc_proc_arch_data_t*)(pcur->arch_data))->cr3;
    proc_proc_current = pnew;
    __asm__ __volatile__("movl %%eax, %%cr3"::"a"(newpd_phys));

    // Load ELF
    // printf("Loading ELF\r\n");
    void (*entry)();
    elf_load_simple(ELF_start, &entry);
    // printf("ELF loaded\r\n");

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

    // Setup process exit information
    pnew->parentwaitingthread = 0;

    pnew->signal_handler = 0;
    pnew->signals = 0;

    return pnew;
}

proc_proc_t * proc_proc_new(void * ELF_start){
    schedule_disable();

    proc_proc_t * pnew;

    unsigned int old_ebp, old_esp;
    __asm__ __volatile__("movl %%ebp, %%eax":"=a"(old_ebp));
    __asm__ __volatile__("movl %%esp, %%eax":"=a"(old_esp));
    unsigned int * newstack = (unsigned int *)(boot_stack_top_C-4);
    *(newstack--) = old_esp;
    *(newstack--) = old_ebp;
    *(newstack) = ELF_start;

    __asm__ __volatile__("movl %%eax, %%esp; movl %%esp, %%ebp"::"a"(newstack));
    __asm__ __volatile__("call *%%eax"::"a"(_0_proc_proc_new));
    __asm__ __volatile__("add $4, %esp ;pop %ebp; pop %esp");
    __asm__ __volatile__("nop":"=a"(pnew));

    message_init_info(&pnew->message_info);

    schedule_enable();
    return pnew;
}

int _0_proc_thread_kill(proc_thread_t * thread, proc_proc_t * process, int retval){
    
    // printf("proc_thread_kill(%08x, %08x, %08x)\r\n", thread, process, retval);
    
    // switch to new VAS
    proc_proc_t * pcur = proc_proc_get_current();
    unsigned int own_vas = ((proc_proc_arch_data_t*)(pcur->arch_data))->cr3;
    unsigned int pvas = ((proc_proc_arch_data_t*)(process->arch_data))->cr3;
    proc_proc_current = process;
    __asm__ __volatile__("movl %%eax, %%cr3"::"a"(pvas));

    // Free kernel stack of thread
    for(int i=0; i<thread->kernel_stack_length/4096; i++){
        vas_unmap_free(thread->kernel_stack+i*4096);
    }

    // switch back to old VAS
    proc_proc_current = pcur;
    __asm__ __volatile__("movl %%eax, %%cr3"::"a"(own_vas));

    // Set return value
    thread->retval = retval;

    // Delete thread from process's thread list
    if(!thread->prev){
        process->threads = thread->next;
        if(thread->next){
            thread->next->prev = 0;
        }
    }else{
        thread->prev->next = thread->next;
        if(thread->next){
            thread->next->prev = thread->prev;
        }
    }
    // Add deleted thread to process's killed thread list
    proc_thread_t * kt = process->killed_threads;
    thread->next = 0;
    if(!kt){
        process->killed_threads = thread;
    }else{
        while(kt->next){
            kt = kt->next;
        }
        kt->next = thread;
    }

    process->threads_number--;

    // Check if process must be deleted
    // TODO move to seperate function
    if(!process->threads_number){
        // printf("kill process %08x\r\n", process);

        if(process->id == 1){
            printf("CANNOT KILL PROCESS 1\r\n");
            for(;;){
                __asm__ __volatile__("cli; hlt");
            }
        }

		// free all mutexes
		mutex_t * mtx = process->mutexes;
		while(mtx){
			mutex_destroy(mtx);
			mtx = mtx->next;
		}

        // Free all thread structures
        proc_thread_t * kt = process->killed_threads;
        while(kt){
            proc_thread_t * next = kt->next;
            kfree(kt);
            kt = next;
        }
        process->killed_threads = 0;

        // Destroy complete VAS
        proc_proc_current = process;
        __asm__ __volatile__("movl %%eax, %%cr3"::"a"(pvas));
        vmm_destroy(&process->memory);
        proc_proc_current = pcur;
        __asm__ __volatile__("movl %%eax, %%cr3"::"a"(own_vas));

        // Free page directory
        vas_map(pvas, 0, VAS_FLAGS_READ);
        unsigned int amount_stack_pagetables = (process->kernel_stacks/4096+1);
        for(int i=0; i<amount_stack_pagetables; i++){
            unsigned int pdphys = *((unsigned int*)(768*4+255*4-4-4*i));
            pdphys &= 0xfffff000;
            vas_map(pdphys, 4096, VAS_FLAGS_READ);
            vas_unmap_free(4096);
        }

        vas_unmap_free(0);

        // Set return value
        process->retvalue = retval;

        if(process->parentwaitingthread){
            schedule_set_state(schedule_get(process->parent->id, process->parentwaitingthread), SCHEDULE_STATE_RUNNING);
        }

    }

    // Check if current process/thread is different than the killed one, then return
    if(thread!=proc_thread_get_current()){
        return 0;
    }

    schedule_enable_completely();
    schedule();
    for(;;);
    return 0;
}

int proc_thread_kill(proc_thread_t * thread, proc_proc_t * process, int retval){
    schedule_disable();

    unsigned int ret = 0;

    unsigned int old_ebp, old_esp;
    __asm__ __volatile__("movl %%ebp, %%eax":"=a"(old_ebp));
    __asm__ __volatile__("movl %%esp, %%eax":"=a"(old_esp));
    unsigned int * newstack = (unsigned int *)(boot_stack_top_C-4);
    *(newstack--) = old_esp;
    *(newstack--) = old_ebp;
    *(newstack--) = retval;
    *(newstack--) = process;
    *(newstack) = thread;

    __asm__ __volatile__("movl %%eax, %%esp; movl %%esp, %%ebp"::"a"(newstack));
    __asm__ __volatile__("call *%%eax"::"a"(_0_proc_thread_kill));
    __asm__ __volatile__("add $12, %esp ;pop %ebp; pop %esp");
    __asm__ __volatile__("nop":"=a"(ret));
    // Should definitly not come here since kernel stack should be freed!!!

    schedule_enable();
    return ret;
}

void _proc_debug_print(proc_proc_t * p, unsigned int indent){
    if(!p){
        return;
    }

    for(int i=0; i<indent; i++) printf("\t");
    printf("-P[%3d]     %08x\r\n", p->id, p);
    for(int i=0; i<indent; i++) printf("\t");
    printf(" cr3:       %08x\r\n", ((proc_proc_arch_data_t*)(p->arch_data))->cr3);
    for(int i=0; i<indent; i++) printf("\t");
    printf(" retval:    %08x\r\n", p->retvalue);
    for(int i=0; i<indent; i++) printf("\t");
    printf(" reason:    %08x\r\n", p->killreason);
    for(int i=0; i<indent; i++) printf("\t");
    printf(" threads:   %d\r\n", p->threads_number);
    proc_thread_t * t = p->threads;
    while(t){
        for(int i=0; i<indent; i++) printf("\t");
        printf(" *T[%3d]    %08x %08x %08x %08x %08x\r\n", t->id, t, t->kernel_stack, t->kernel_stack_length, t->stack, t->stack_length);
        t = t->next;
    }
    t = p->killed_threads;
    while(t){
        for(int i=0; i<indent; i++) printf("\t");
        printf(" *KT[%3d]   -> %08x %08x\r\n", t->id, t, t->retval);
        t = t->next;
    }
    for(int i=0; i<indent; i++) printf("\t");
    printf(" childs:\r\n");
    proc_proc_t * c = p->childs;
    while(c){
        _proc_debug_print(c, indent+1);
        c = c->child_next;
    }
}

void proc_debug(){
    printf("## Processes:\r\n");
    proc_proc_t * p = proc_get(1);
    _proc_debug_print(p, 0);
}

proc_proc_t * _proc_get(proc_proc_t * p, unsigned int pid){
    if(p->id == pid){
        return p;
    }
    // Loop over childs
    proc_proc_t * c = p->childs;
    while(c){
        proc_proc_t * r = _proc_get(c, pid);
        if(r)
            return r;
        c = c->child_next;
    }
    return 0;
}

proc_proc_t * proc_get(unsigned int pid){
    proc_proc_t * p = proc_proc_get_current();
    // Get to pid 1
    while(p->parent)    
        p = p->parent;

    proc_proc_t * ret = _proc_get(p, pid);
    return ret;
}

proc_thread_t * proc_thread_get(unsigned int tid, unsigned int pid){
    proc_proc_t * p = proc_get(pid);
    // Loop over threads
    proc_thread_t * t = p->threads;
    while(t){
        if(t->id == tid)
            return t;
        t = t->next;
    }
    return 0;
}

