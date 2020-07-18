#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv){
	char * s = "This is testbin.bin\r\n";
	int len = strlen(s);
	write(0, s, len);
	return 0;
}
