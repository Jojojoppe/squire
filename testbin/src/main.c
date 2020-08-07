#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv){
	printf("This is testbin!\r\n");
	for(int i=0; i<argc; i++)
		printf("%d: %s\r\n", i, argv[i]);
	return 0;
}
