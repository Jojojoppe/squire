#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")

#undef errno
extern int errno;

char *__env[1] = {0};
char **environ = __env;

void _exit(){
	for(;;);
}

int close(int file){
	return -1;
}

int execve(char *name, char **argv, char **env){
	errno = ENOMEM;
	return -1;
}

int fork(){
	errno = EAGAIN;
	return -1;
}

int fstat(int file, struct stat *st){
	st->st_mode = S_IFCHR;
	return 0;
}

int getpid(){
	return 1;
}

int isatty(int file){
	return 1;
}

int kill(int pid, int sig){
	errno = EINVAL;
	return -1;
}

int link(char *old, char *new){
	errno = EMLINK;
	return -1;
}

int lseek(int file, int ptr, int dir){
	return 0;
}

int open(const char *name, int flags, ...){
	return -1;
}

int read(int file, char *ptr, int len){
	return 0;
}

caddr_t sbrk(int incr){
	return (caddr_t)0;
}

int stat(const char *file, struct stat *st){
	st->st_mode = S_IFCHR;
	return 0;
}

clock_t times(struct tms *buf){
	return -1;
}

int unlink(char *name){
	errno = ENOENT;
	return -1;
}

int wait(int *status){
	errno = ECHILD;
	return -1;
}

int write(int file, char *ptr, int len){
	// Use LOG syscall for now
	unsigned int parms[2] = {(unsigned int)ptr, len};
	asm __volatile__("int $0x80"::"a"(0x10000000),"c"(8),"d"(parms));
	return len;
}

#pragma GCC pop_options
