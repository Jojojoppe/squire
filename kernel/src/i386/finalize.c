#include <general/kprintf.h>

unsigned int finalize_boot_finalized = 0;

void finalize(){
	printf("- Releasing boot resources: \r\n");
	printf("   + serial\r\n");
	finalize_boot_finalized = 1;
}

void finalize_fatal_error(){
	printf("- Retaking boot resources\r\n");
	printf("   + serial\r\n");
	finalize_boot_finalized = 0;
	asm volatile("call serial_init");
}
