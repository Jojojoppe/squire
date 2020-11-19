#include "fsdriver_initramfs.h"

#include <squire.h>
#include <squire_fsdriver.h>

#include <string.h>
#include "tar.h"

void * tar_start;

int fslsdir_callback(int * r_nr_nodes, size_t * r_shared_size, void ** r_shared, void * private_mount, void * private_dir, char shared_id[32]){

	// List over tar to get nodelist size
	tar_header_t * hdr = (tar_header_t*) tar_start;
	unsigned int nodelist_size = 0;
	while(hdr->filename[0]){

		nodelist_size += sizeof(vfs_lsdir_node_t);
	
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

	// Get shared size
	size_t shared_size = ((nodelist_size/4096 + 1)*4096);
	vfs_lsdir_node_t * shared_nodelist = (vfs_lsdir_node_t*) squire_memory_create_shared(0, shared_size, shared_id, MMAP_READ|MMAP_WRITE);
	
	// Fill nodelist
	hdr = (tar_header_t*) tar_start;
	unsigned int i = 0;
	while(hdr->filename[0]){

		shared_nodelist[i].fsdriver_private = (void*)hdr;
		shared_nodelist[i].owner = 0;
		shared_nodelist[i].permissions = PERMISSIONS_EXECALL|PERMISSIONS_EXECOWN|PERMISSIONS_READALL|PERMISSIONS_READOWN;
		shared_nodelist[i].type = VFS_NODE_TYPE_FILE;
		strcpy(shared_nodelist[i].name, hdr->filename);

		// get size
		unsigned int count = 1;
		unsigned int size = 0;
		for(int j=11; j>0; j--, count*=8)
			size += ((hdr->size[j-1]-'0')*count);

		// Next header
		unsigned int nxt = (unsigned int)hdr + ((size/512)+1)*512;
		if(size%512) nxt += 512;
		hdr = (tar_header_t*)nxt;
		i++;
	}

	*r_nr_nodes = i;
	*r_shared_size = shared_size;
	*r_shared = shared_nodelist;

	return 0;
}

int fsmount_callback(void ** r_private_mount, void ** r_private_root, uint8_t mountpoint, char fsnane[32], uint8_t device_id[64], uint32_t device_instance){

	*r_private_mount = 0;
	*r_private_root = 0;

	return 0;
}

// --------------------------------------------------

squire_fsdriver_t fsdriver = {
	"init-fsdrivers", 1, 0,
	10,
	{
		{"initramfs", VFS_FSDRIVER_FLAGS_NODEVICE | VFS_FSDRIVER_FLAGS_NOCACHE},
		{0},
	},
};
SQUIRE_FSDRIVER_INFO fsdriver_info = {
	&fsdriver,
	fsmount_callback,
	fslsdir_callback,
};
SQUIRE_FSDRIVER_INFO * _fsdriver_info = &fsdriver_info;

int initramfs_fsdriver(void * p){
	tar_start = p;
	char ** argv_a[3];
	char * argv0 = "initramfs"; argv_a[0] = argv0;
	char * argv1 = "1"; argv_a[1] = argv1;
	char * argv2 = "1"; argv_a[2] = argv2;
	atoi("0");
	return fsdriver_main(3, argv_a);
}
