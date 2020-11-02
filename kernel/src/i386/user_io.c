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
