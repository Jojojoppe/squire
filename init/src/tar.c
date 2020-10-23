#include "tar.h"

void tar_print_list(void * start){
	tar_header_t * hdr = (tar_header_t *) start;
	while(hdr->filename[0]){
		printf("tar: %s\r\n", hdr->filename);

		// get size
		uint32_t count = 1;
		uint32_t size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		uint32_t nxt = (uint32_t)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}
}

void * tar_get(void * start, char * fname, uint32_t * fsize){
	tar_header_t * hdr = (tar_header_t *) start;
	while(hdr->filename[0]){

		// Check filename
		char * s = fname;
		char * d = hdr->filename;
		uint32_t same = 1;
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
		uint32_t count = 1;
		uint32_t size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		if(same){
			if(size){
				*fsize = size;
			}
			return (void*)&hdr[1];
		}

		// Next header
		uint32_t nxt = (uint32_t)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
	}
	return 0;
}