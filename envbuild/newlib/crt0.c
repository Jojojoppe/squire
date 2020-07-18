extern int main(int argc, char ** argv);

void _start(){
	main(0, (char*)0);
	for(;;);
}
