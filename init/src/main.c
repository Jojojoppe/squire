#include <stdio.h>
#include <squire.h>

int main(int argc, char ** argv){
	unsigned long i;
	for(;;){
		printf("\r[%08d] init.bin\r", i++);
	}
}
