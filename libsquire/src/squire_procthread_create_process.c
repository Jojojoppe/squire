#include <squire_syscall.h>

extern void * malloc(size_t);
extern void free(void *);

unsigned __squire_strlen(const char * s){
	unsigned len=0;
	while(s[len++]);
	return len;
}

void __squire_strcpy(char * dest, const char * src){
	int i = 0;
	while (1) {
		dest[i] = src[i];
		if (dest[i] == '\0') break;
		i++;
	}
}

unsigned int squire_procthread_create_process(void * elf_start, size_t elf_length, int argc, char ** argv){

    // Get parameter data length
	unsigned int param_length = 0;
	for(int i=0; i<argc; i++){
		param_length += __squire_strlen(argv[i]) + 1;
	}
	unsigned int * paramdata = (unsigned int*) malloc(param_length+sizeof(unsigned int)*argc);
	unsigned int p = 0;
	for(int i=0; i<argc; i++){
		unsigned int len = __squire_strlen(argv[i])+1;
		char * d = (char*)((unsigned int)paramdata + sizeof(unsigned int)*argc + p);
		paramdata[i] = len;
		p += len;
		__squire_strcpy(d, argv[i]);
	}

    squire_syscall_procthread_t params;
    params.operation = SQUIRE_SYSCALL_PROCTHREAD_OPERATION_CREATE_PROCESS;
    params.address0 = elf_start;
    params.length0 = elf_length;
    params.value0 = argc;
    params.address1 = paramdata;
    params.length1 = param_length+argc*sizeof(unsigned int);
    squire_syscall(SQUIRE_SYSCALL_PROCTHREAD, sizeof(params), &params);
    free(paramdata);
    return params.pid0;
}