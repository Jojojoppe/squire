#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

void SIGINTRhandler(int sig){
	if(squire_extraval0 == 0x24){
		unsigned status;
		squire_io_port_inb(0x3f8+5, &status);
		if(status&1){
			char c;
			squire_io_port_inb(0x3f8, &c);
			printf("%c\r\n", c);
		}
		squire_io_port_outb(0x20, 0x20);
	}
}

void SIGTIMhandler(int sig){
	printf("SIGTIM\r\n");
	asm("cli");
}

int main(int argc, char ** argv){
	signal(SIGINTR, SIGINTRhandler);
	signal(SIGTIM, SIGTIMhandler);
	squire_misc_finalize();
	printf("Main thread of init.bin\r\n");

	squire_io_register_isr(0x24);
	squire_io_register_port(0x3f8, 8, IO_PORT_WRITE|IO_PORT_READ);
	squire_io_register_port(0x20, 2, IO_PORT_WRITE|IO_PORT_READ);

	// Enable COM1 interrupts on UART
	squire_io_port_outb(0x3f8+1, 1);

	// Enable COM1 interrupts in PIC
	unsigned char pic1_mask;
	squire_io_port_inb(0x21, &pic1_mask);
	squire_io_port_outb(0x21, pic1_mask & ~(1<<4));

	squire_misc_timer_add(500, 0);

	for(;;);
	return 0;
} 
