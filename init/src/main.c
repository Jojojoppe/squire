#include <squire.h>
#include <fdlibm.h>

#include "tar.h"

int main(int argc, char ** argv){
	printf("This is init.bin!!!\r\n");

	double a = 12.5f;
	double b = 2.0f;
	double c = sin(a*b);
	unsigned int * uic = &c;
	printf("%08x%08x\r\n", *(uic+1), *(uic));

	for(;;);
	return 0;
}
 
