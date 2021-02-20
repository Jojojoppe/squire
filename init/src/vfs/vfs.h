#ifndef __H_VFS
#define __H_VFS 1

#include <stdint.h>

typedef struct vfs_fsdriver_s{
	struct vfs_fsdriver_s * next;
	char name[64];
	uint16_t version_major, version_minor;
	uint8_t id[64];
	uint32_t flags;
	unsigned int PID;
	unsigned int simple_box;
} vfs_fsdriver_t;

typedef struct vfs_mountpoint_s{
	vfs_fsdriver_t * fsdriver;
	void * private_mount;
	void * private_root;
} vfs_mountpoint_t;

int vfs_main(void * p);

#endif
