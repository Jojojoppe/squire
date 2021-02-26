#ifndef _H_INITRAMFS
#define _H_INITRAMFS 1

#include <stddef.h>

typedef struct{
    char name[128];
    unsigned char * base;
    size_t length;
} initramfs_file_t;

int initramfs_main(void * p);

#endif