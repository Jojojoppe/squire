#pragma once

#include <stdint.h>

typedef struct{
	char		filename[100];
	char		mode[8];
	char		uid[8];
	char		gid[8];
	char		size[12];
	char		mtime[12];
	char		chksum[8];
	char		typeflag[1];
	char		pad[355];
} tar_header_t;

void tar_print_list(void * start);
void * tar_get(void * start, char * fname, uint32_t * size);