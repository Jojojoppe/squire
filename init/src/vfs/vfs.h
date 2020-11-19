#ifndef __H_VFS
#define __H_VFS 1

#include <stdint.h>
#include <stddef.h>
#include <squire_fsdriver.h>

#define MSG_BOX_VFS_SIMPLE 1

typedef struct vfs_node_s{
	// Basic information
	vfs_node_type_t type;
	char name[128];

	unsigned int permissions;
	unsigned int owner;

	void * fsdriver_private;

	// Linkage
	struct vfs_node_s * next;
	struct vfs_node_s * prev;
	struct vfs_node_s * childs;
	struct vfs_node_s * parent;
} vfs_node_t;

typedef struct vfs_fsdriver_s{
	struct vfs_fsdriver_s * next;
	char name[32];
	uint8_t flags;
	unsigned int pid;
	unsigned int box;
} vfs_fsdriver_t;

typedef struct{
	unsigned int permissions;
	vfs_fsdriver_t * fsdriver;
	// If fsdriver does not have FLAGS_NODEVICE
	uint8_t device_id[64];
	uint32_t device_instance;
	// Superblock information
	void * fsdriver_private;
	// Root node for VFS tree
	vfs_node_t * rootnode;						// Zero if not initialized
	unsigned int owner;
} vfs_mountpoint_t;

int vfs_main(void * p);

#endif
