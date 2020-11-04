#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <squire.h>

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

int main(int argc, char ** argv){
	printf("Main thread of init.bin\r\n");

	for(;;);
	return 0;
} 
