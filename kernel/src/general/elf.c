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
            size_t length = pheader[i].p_memsz + (0x1000-(pheader[i].p_memsz%0x1000));
            if(vmm_alloc(pheader[i].p_vaddr, length, VMM_FLAGS_EXEC | VMM_FLAGS_READ | VMM_FLAGS_WRITE, &memory)){
                printf("Could not allocate memory\r\n");
                return ELF_ERROR_MEMORY;
            }
            memset(pheader[i].p_vaddr,0, length);
            memcpy(pheader[i].p_vaddr,address+pheader[i].p_offset,pheader[i].p_filesz);
        }
    }

    *entry = eheader2->e_entry;

    return 0;
}