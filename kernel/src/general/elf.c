#include <general/elf.h>
#include <general/arch/proc.h>
#include <general/vmm.h>
#include <general/string.h>

unsigned int elf_load_simple(void * address, void (**entry)()){

    elf_header_t * eheader = (elf_header_t*)address;

    // Check if file is elf file
    if(eheader->EI_MAG[0]!=0x7f || eheader->EI_MAG[1]!='E' || eheader->EI_MAG[2]!='L' || eheader->EI_MAG[3]!='F')
        return ELF_ERROR_NO_ELF_FILE;

    // Check if file is compatible
    if(eheader->EI_CLASS!=ELF_CLASS)
        return ELF_ERROR_WRONG_CLASS;
    if(eheader->EI_DATA!=ELF_DATA)
        return ELF_ERROR_WRONG_ENDIANNESS;
    if(eheader->e_machine!=ELF_ISA)
        return ELF_ERROR_WRONG_ISA;
    if(eheader->e_type!=0x02)
        return ELF_ERROR_NOT_EXECUTABLE;
    
    elf_header2_t * eheader2 = (elf_header2_t*)(address + sizeof(elf_header_t));
    vmm_region_t * memory = proc_get_memory();

    // Get program header
    elf_program_header_t * pheader = (elf_program_header_t*)(address + eheader2->e_phoff);
    for(int i=0; i<eheader2->e_phnum; i++){
        // If loadable segment
        if(pheader[i].p_type==1){
            size_t length = pheader[i].p_memsz;
            unsigned int base = pheader[i].p_vaddr & ~((unsigned int)PAGE_SIZE-1);
            length += pheader[i].p_vaddr - base;
            if(length%PAGE_SIZE){
                length += (PAGE_SIZE-(length%PAGE_SIZE));
            }

            // Find flags for allocation
            unsigned int flags = 0;
            if(pheader[i].p_flags&1) flags |= VMM_FLAGS_EXEC;
            if(pheader[i].p_flags&2) flags |= VMM_FLAGS_WRITE;
            if(pheader[i].p_flags&4) flags |= VMM_FLAGS_READ;
            if(vmm_alloc(base, length, flags, &memory)){
                printf("Could not allocate memory [%08x] %08x\r\n", base, length);
                vmm_debug(memory);
                return ELF_ERROR_MEMORY;
            }
            memset(base,0, length);
            memcpy(pheader[i].p_vaddr,address+pheader[i].p_offset,pheader[i].p_filesz);
        }
    }

    *entry = eheader2->e_entry;
    proc_set_memory(memory);

    return 0;
}