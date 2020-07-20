#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv){
	printf("This is testbin! argc=%d argv[0]=%s argv[1]=%08x\r\n", argc, argv[0], argv[1]);
	return 0;
}
