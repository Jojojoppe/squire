#ifndef __H_ELF
#define __H_ELF 1

/**
 * @file
 * @section DESCRIPTION
 * ELF loading functions
 */
#include <general/stdint.h>
#include <general/config.h>

typedef struct{
    char            EI_MAG[4];
    uint8_t         EI_CLASS;
    uint8_t         EI_DATA;
    uint8_t         EI_VERSION;
    uint8_t         EI_OSABI;
    uint8_t         EI_ABIVERSION;
    uint8_t         EI_PAD[7];
    uint16_t        e_type;
    uint16_t        e_machine;
    uint32_t        e_version;
} __attribute__((packed)) elf_header_t;

#if ELF_CLASS == 1
typedef struct{
    uint32_t        e_entry;
    uint32_t        e_phoff;
    uint32_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} __attribute__((packed)) elf_header2_t;
#elif ELF_CLASS == 2
typedef struct{
    uint64_t        e_entry;
    uint64_t        e_phoff;
    uint64_t        e_shoff;
    uint32_t        e_flags;
    uint16_t        e_ehsize;
    uint16_t        e_phentsize;
    uint16_t        e_phnum;
    uint16_t        e_shentsize;
    uint16_t        e_shnum;
    uint16_t        e_shstrndx;
} __attribute__((packed)) elf_header2_t;
#else
typedef void elf_header2_t;
#endif

#if ELF_CLASS == 1
typedef struct{
    uint32_t        p_type;
    uint32_t        p_offset;
    uint32_t        p_vaddr;
    uint32_t        p_paddr;
    uint32_t        p_filesz;
    uint32_t        p_memsz;
    uint32_t        p_flags;
    uint32_t        p_align;
} __attribute__((packed)) elf_program_header_t;
#elif ELF_CLASS == 2
typedef struct{
    uint32_t        p_type;
    uint32_t        p_flags;
    uint64_t        p_offset;
    uint64_t        p_vaddr;
    uint64_t        p_paddr;
    uint64_t        p_filesz;
    uint64_t        p_memsz;
    uint64_t        p_align;
} __attribute__((packed)) elf_program_header_t;
#else
typedef void elf_program_header_t;
#endif

#if ELF_CLASS == 1
typedef struct{
    uint32_t        sh_name;
    uint32_t        sh_type;
    uint32_t        sh_flags;
    uint32_t        sh_addr;
    uint32_t        sh_offset;
    uint32_t        sh_size;
    uint32_t        sh_link;
    uint32_t        sh_info;
    uint32_t        sh_addralign;
    uint32_t        sh_entsize;
} __attribute__((packed)) elf_section_header_t;
#elif ELF_CLASS == 2
typedef struct{
    uint32_t        sh_name;
    uint32_t        sh_type;
    uint64_t        sh_flags;
    uint64_t        sh_addr;
    uint64_t        sh_offset;
    uint64_t        sh_size;
    uint32_t        sh_link;
    uint32_t        sh_info;
    uint64_t        sh_addralign;
    uint64_t        sh_entsize;
} __attribute__((packed)) elf_section_header_t;
#else
typedef void elf_section_header_t;
#endif

enum ELF_ERROR{
    ELF_ERROR_NO_ELF_FILE = 1,
    ELF_ERROR_WRONG_CLASS,
    ELF_ERROR_WRONG_ENDIANNESS,
    ELF_ERROR_WRONG_ISA,
    ELF_ERROR_NOT_EXECUTABLE,
    ELF_ERROR_MEMORY
};

/**
 * @brief Load ELF file without dynamic linking
 * 
 * @param address Starting address of ELF file
 * @param entry Address of variable which stores entry
 * @return Zero if successful
 */
unsigned int elf_load_simple(void * address, void (**entry)());

#endif