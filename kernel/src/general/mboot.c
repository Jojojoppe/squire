#include <general/mboot.h>
#include <general/kprintf.h>
#include <general/string.h>

unsigned int mboot_get_mod(const char * name, void ** address, size_t * length){

    // Get module list
    extern mboot_info_t * MBOOT_header;
    extern unsigned int MBOOT_offset;
    mboot_mod_t * mods = (mboot_mod_t*)(MBOOT_header->moduleAddress+MBOOT_offset);
    unsigned int modcount = MBOOT_header->moduleCount;

    for(int i=0; i<modcount; i++){
        char * modname = (char *)(mods[i].string + MBOOT_offset);
        if(!strcmp(name, modname)){
            *length = mods[i].end-mods[i].start;
            *address = (void*)(mods[i].start+MBOOT_offset);
            return 0;
        }
    }

    return 1;
}