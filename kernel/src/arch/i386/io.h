#ifndef __H_IO
#define __H_IO 1

void io_outb(unsigned int address, unsigned char b);
void io_outw(unsigned int address, unsigned short b);
void io_outd(unsigned int address, unsigned int b);
unsigned char io_inb(unsigned int address);
unsigned short io_inw(unsigned int address);
unsigned int io_ind(unsigned int address);

#endif