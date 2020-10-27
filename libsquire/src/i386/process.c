#include "../include/squire.h"

unsigned _strlen(const char * s){
	unsigned len=0;
	while(s[len++]);
	return len;
}

void _strcpy(char * dest, const char * src){
	int i = 0;
	while (1) {
		dest[i] = src[i];
		if (dest[i] == '\0') break;
		i++;
	}
}

unsigned int squire_syscall_process(void * elf_start, size_t elf_length, int argc, char ** argv){

	// Get parameter data length
	unsigned int param_length = 0;
	for(int i=0; i<argc; i++){
		param_length += _strlen(argv[i]) + 1;
	}
	unsigned int * paramdata = (unsigned int*) malloc(param_length+sizeof(unsigned int)*argc);
	unsigned int p = 0;
	for(int i=0; i<argc; i++){
		unsigned int len = _strlen(argv[i])+1;
		char * d = (char*)((unsigned int)paramdata + sizeof(unsigned int)*argc + p);
		paramdata[i] = len;
		p += len;
		_strcpy(d, argv[i]);
	}

	squire_params_process_t parms;
	parms.elf_start = elf_start;
	parms.elf_length = elf_length;
	parms.argc = argc;
	parms.param_data = paramdata;
	parms.param_data_size = param_length+argc*sizeof(unsigned int);
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_PROCESS),"c"(sizeof(parms)),"d"(&parms));
	free(paramdata);
	return parms.pid;
}