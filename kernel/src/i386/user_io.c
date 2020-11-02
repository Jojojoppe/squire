#include <general/arch/user_io.h>
#include <general/kmalloc.h>
#include <general/string.h>

int user_io_arch_may_register_isr(unsigned int id){
	// Exceptions and PIC0 can't be registerd
	if(id<=30 || id==0x20)
		return 1;
	return 0;
}

void user_io_arch_register_isr(unsigned int nr, unsigned int PID){
	extern char isr_user_start;
	extern char isr_user_end;
	size_t isr_user_length = &isr_user_end - &isr_user_start;
	// Create new space for usr isr
	char * usrisr = kmalloc(isr_user_length);
	memcpy(usrisr, &isr_user_start, isr_user_length);
	// Set interrupt id
	unsigned int * id = (unsigned int*)(usrisr+3);
	*id = nr;
	// Set PID
	unsigned int * pid = (unsigned int*)(usrisr+9);
	*pid = PID;
	// Set the interrupt
	idt_set_interrupt_user_c(nr, usrisr);
}

int user_io_arch_may_register_port(unsigned int port, unsigned int flags){
	// TODO check which ports to disable (PIT?)
	return 0;
}

void user_io_arch_register_port(unsigned int port, unsigned int flags, unsigned int PID){
}

void user_io_arch_outb(unsigned int address, unsigned char val){
	io_outb(address, val);
}
void user_io_arch_outw(unsigned int address, unsigned short val){
	io_outw(address, val);
}
void user_io_arch_outd(unsigned int address, unsigned int val){
	io_outd(address, val);
}
unsigned char user_io_arch_inb(unsigned int address){
	return io_inb(address);
}
unsigned short user_io_arch_inw(unsigned int address){
	return io_inw(address);
}
unsigned int user_io_arch_ind(unsigned int address){
	return io_ind(address);
}


