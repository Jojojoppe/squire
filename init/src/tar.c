#include "tar.h"

#include <string.h>

void tar_print_list(void * start){
	tar_header_t * hdr = (tar_header_t *) start;
	while(hdr->filename[0]){
		printf("tar: %s\r\n", hdr->filename);

		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}
}

void * tar_get(void * start, char * fname, unsigned int * fsize){
	tar_header_t * hdr = (tar_header_t *) start;
	while(hdr->filename[0]){

		// Check filename
		char * s = fname;
		char * d = hdr->filename;
		unsigned int same = 1;
		while(1){
			if(*s!=*d){
				same = 0;
				break;
			}
			if(*s==0 && *d==0){
				break;
			}
			s++;
			d++;
		}

		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		if(same){
			if(size){
				*fsize = size;
			}
			return (void*)&hdr[1];
		}

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}
	return 0;
}

int tar_exists(void * start, char * fname){
	tar_header_t * hdr = (tar_header_t *) start;
	int exists = 0;
	while(hdr->filename[0]){

		if(!strcmp(hdr->filename, fname)){
			exists = 1;
		}

		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}
	return exists;
}