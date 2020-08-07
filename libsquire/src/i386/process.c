#include "../include/squire.h"

int slen(char * s){
	int len = 0;
	while(*(s++))
		len++;
	return len;
}

void scpy(char * d, char * s){
	while(*s){
		*(d++) = *(s++);
	}
	*d = 0;
}

uint32_t squire_syscall_process(void * elf_start, size_t elf_length, int argc, char ** argv){
	int paramsize = argc*4;
	for(int i=0; i<argc; i++){
		paramsize += slen(argv[i])+1;
	}
	char * paramdata = alloca(paramsize);
	char * p = paramdata;
	for(int i=0; i<argc; i++){
		unsigned int len = slen(argv[i])+1;
		*((unsigned int*)p) = len;
		p += 4;
		scpy(p, argv[i]);
		p += len;
	}

	squire_params_process_t parms;
	parms.elf_start = elf_start;
	parms.elf_length = elf_length;
	parms.argc = argc;
	parms.param_data = paramdata;
	parms.param_data_size = paramsize;
	asm __volatile__("int $0x80"::"a"(SQUIRE_SYSCALL_PROCESS),"c"(sizeof(parms)),"d"(&parms));
	return parms.elf_start;
}