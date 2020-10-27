#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <squire.h>

int main(int argc, char ** argv){
	printf("This is testbin!!!\r\n");
	printf("argc = %08x argv = %08x\r\n", argc, argv);
	for(int i=0; i<argc; i++)
		printf("argv[%d] = %s\r\n", i, argv[i]);

	for(int i=0; i<100000000; i++);

	return 0xffeeddcc;
}
