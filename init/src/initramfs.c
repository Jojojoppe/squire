#include "initramfs.h"
#include <squire.h>
#include <squire_vfs.h>

#include <dirent.h>
#include <stdio.h>
#include "tar.h"

void * initramfs_tar_start;
initramfs_file_t * files;
int initramfs_count = 0;

int fs_mount(char * type, char * device, unsigned int mountpoint, unsigned int flags){
    // Create files list
    initramfs_count = tar_count(initramfs_tar_start);
    files = (initramfs_file_t*)malloc(sizeof(initramfs_file_t)*initramfs_count);
    // Read files list
    for(int i=0; i<initramfs_count; i++){
        files[i].base = tar_get_info(initramfs_tar_start, i, &files[i].name, &files[i].length);
    }
    return 0;
}

int fs_unmount(unsigned int mountpoint){
    free(files);
    files = 0;
    return 0;
}

int fs_opendir(char * path, struct dirent * dirent){
    dirent->d_fileno = 0;
    return 0;
}

int fs_readdir(unsigned int current_entry, struct dirent * dirent){
    if(current_entry>=initramfs_count) return -1;
    dirent->d_fileno = current_entry;
    strcpy(dirent->d_name, files[current_entry].name);
    return 0;
}

int fs_open(char * path, unsigned int * fd){
    for(int i=0; i<initramfs_count; i++){
        if(!strcmp(path+1, files[i].name)){
            *fd = i;
            return 0;
        }
    }
    return -1;
}

int fs_read(unsigned int fd, size_t offset, size_t * length, char * buf){
    if(fd>=initramfs_count) return ;
    if(offset>=files[fd].length) return -1;
    if(offset+*length>=files[fd].length){
        *length = files[fd].length-offset;
    }
    memcpy(buf, files[fd].base+offset, *length);
    return 0;
}

squire_vfs_driver_t initramfs_driver_info = {
    "INITRAMFS", 1, 0,
    0, 30,
    fs_mount,
    fs_unmount,
    fs_opendir,
    fs_readdir,
    fs_open,
    fs_read,
    {
        {"INITRAMFS"}
    }
};
int initramfs_main(void * p){
    initramfs_tar_start = p;
    squire_vfs_driver_main_direct(0, 0, &initramfs_driver_info);
    for(;;);
    return 0;
}