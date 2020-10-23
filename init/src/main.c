#include <stdio.h>
#include <squire.h>

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");
	printf("argc: %08x argv: %08x\r\n", argc, argv);
	printf("argv[0] = %s\r\n", argv[0]);
	printf("argv[1] = %08x\r\n", *((unsigned int*)argv[1]));

	return 0;
}
 