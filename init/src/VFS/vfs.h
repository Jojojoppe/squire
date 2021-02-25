#ifndef __H_VFS
#define __H_VFS 1

#include <stdint.h>
#include <squire_vfs.h>

typedef struct vfs_driver_s{
    struct vfs_driver_s * next;
    squire_vfs_driver_t driver_info;
    unsigned int pid;
} vfs_driver_t;

int vfs_main(void * p);

#endif;