#ifndef __H_X86_GENERIC_PCI
#define __H_X86_GENERIC_PCI 1

#include <squire.h>

void x86_generic_PCI_init(char * device);
void x86_generic_PCI_enum(char * device);
void x86_generic_PCI_idm(unsigned int function, void * data, size_t length, unsigned int from, unsigned int box);

#endif