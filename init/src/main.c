#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <threads.h>
#include <dirent.h>
#include <errno.h>

#include <squire.h>
#include <squire_vfs.h>

#include "tar.h"
#include "DDM/ddm.h"
#include "VFS/vfs.h"
#include "initramfs.h"

#include <squire_crypt.h>

void tree(char * path){
	DIR * d = opendir(path);
	if(!d) return;

	struct dirent * dire = readdir(d);
	while(dire){
		printf("    > %s\r\n", dire->d_name);
		dire = readdir(d);
	}

	closedir(d);
}

int main(int argc, char ** argv){
	printf("Main thread of init.bin, argc=%d\r\n", argc);

	// Get current process information
	unsigned int PID = squire_procthread_getpid();
	void * tar_start = (void*)(*((unsigned int*)argv[1]));

	// Start VFS
	thrd_t thrd_vfs;
	thrd_create(&thrd_vfs, vfs_main, 0);

	// Start DDM
	thrd_t thrd_ddm;
	thrd_create(&thrd_ddm, ddm_main, 0);

	// Start initramfs driver
	thrd_t thrd_initramfs;
	thrd_create(&thrd_initramfs, initramfs_main, tar_start);

	// Mount initramfs
	for(int i=0; i<0x2000000; i++); 
	if(squire_vfs_user_mount("INITRAMFS", "", SQUIRE_MP_INITRAMFS)){
		printf("Could not mount initramfs\r\n");
		for(;;);
	}
	printf("Mounted initramfs\r\n");


	FILE * f_licence = fopen("1:/LICENCE", "r");
	if(!f_licence){
		printf("ERROR: could not open 1:/LICENCE\r\n");
		for(;;);
	}
	printf("1:/LICENCE opened\r\n");
	char buf[1024+1];
	size_t r;
	do{
		r = fread(buf, 1, 1024, f_licence);
		buf[r] = 0;
		// printf("%s\r\n", buf);
	} while(r>0);
	fclose(f_licence);

	// unsigned int root_device_driver_size;
	// void * root_device_driver = tar_get(tar_start, "x86_generic.bin", &root_device_driver_size);
	// char ** rdd_argv[1];
	// rdd_argv[0] = "x86_generic";
	// squire_procthread_create_process(root_device_driver, root_device_driver_size, 1, rdd_argv);

	// unsigned int ide_device_driver_size;
	// void * ide_device_driver = tar_get(tar_start, "x86_ide.bin", &ide_device_driver_size);
	// char ** idd_argv[1];
	// idd_argv[0] = "x86_ide";
	// squire_procthread_create_process(ide_device_driver, ide_device_driver_size, 1, idd_argv);

	// unsigned int vga_device_driver_size;
	// void * vga_device_driver = tar_get(tar_start, "generic_vga.bin", &vga_device_driver_size);
	// char ** vdd_argv[1];
	// vdd_argv[0] = "generic_vga";
	// squire_procthread_create_process(vga_device_driver, vga_device_driver_size, 1, vdd_argv);

	// for(int i=0; i<0x8000000; i++); 
	// squire_vfs_user_mount("DDM_FS", "", 0);
	// // Print tree of DDM_FS
	// tree("0:/");

	for(;;);
	return 0;
} 
