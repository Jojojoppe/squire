void io_outb(unsigned int address, unsigned char b){
    asm volatile("out %%al, %%dx"::"d"(address),"a"(b));
}
void io_outw(unsigned int address, unsigned short b){
    asm volatile("out %%ax, %%dx"::"d"(address),"a"(b));
}
void io_outd(unsigned int address, unsigned int b){
    asm volatile("out %%eax, %%dx"::"d"(address),"a"(b));
}
unsigned char io_inb(unsigned int address){
    unsigned char ret;
    asm volatile("in %%dx, %%al":"=a"(ret):"d"(address));
    return ret;
}
unsigned short io_inw(unsigned int address){
    unsigned short ret;
    asm volatile("in %%dx, %%ax":"=a"(ret):"d"(address));
    return ret;
}
unsigned int io_ind(unsigned int address){
    unsigned int ret;
    asm volatile("in %%dx, %%eax":"=a"(ret):"d"(address));
    return ret;
}