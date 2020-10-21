#include <stdio.h>
#include <squire.h>

int main(int argc, char ** argv){
	squire_syscall_mmap(0x05000000, 0x1000, 0);
	squire_syscall_mmap(0x05100000, 0x1000, 0);
	for(;;);
}
