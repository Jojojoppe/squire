#include <general/elf.h>

unsigned int elf_load_simple(void * address){

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

    return 0;
}