extern unsigned int finalize_boot_finalized;

int putchar(char c){
	if(!finalize_boot_finalized)
		asm volatile("movl 8(%ebp), %eax; call serial_out");
    return c;
}
