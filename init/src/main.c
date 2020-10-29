#include <squire.h>
#include <fdlibm.h>

#include "tar.h"

int main(int argc, char ** argv){
	printf("This is init.bin!!!\r\n");

	printf("sin(0.5*3.1415)=%6f\r\n", sin(0.5f*3.1415f));

	for(;;);
	return 0;
}
 
