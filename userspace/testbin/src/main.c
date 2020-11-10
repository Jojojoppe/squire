#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_driver.h>

int main(int argc, char ** argv){
	printf("This is testbin!\r\n");


	// Map shared region
	char shared_id[32] = "0123456789";
	void * shared = squire_memory_map_shared(0, 1, shared_id, MMAP_READ | MMAP_WRITE);
	printf("%s\r\n", shared);

	for(;;);
	return EXIT_SUCCESS;
}
