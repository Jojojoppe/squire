#ifndef __H_VFS
#define __H_VFS 1

#include <stdint.h>

#define MSG_BOX_VFS_SIMPLE 1

// File permissions
#define PERMISSIONS_READALL 1
#define PERMISSIONS_WRITEALL 2
#define PERMISSIONS_EXECALL 4
#define PERMISSIONS_READOWN 8
#define PERMISSIONS_WRITEOWN 16
#define PERMISSIONS_EXECOWN 32

typedef enum VFS_NODE_TYPE{
	VFS_NODE_TYPE_FILE,
	VFS_NODE_TYPE_FOLDER,
} vfs_node_type_t;

typedef struct vfs_node_s{
	// Basic information
	vfs_node_type_t type;
	char name[128];

	unsigned int permissions;
	unsigned int owner;

	// File Operations
	unsigned int open_pid, open_box;			// Open file
	unsigned int close_pid, close_box;			// Close file
	unsigned int read_pid, read_box;			// Read file
	unsigned int write_pid, write_box;			// Write file
	unsigned int seek_pid, seek_box;			// Change position in file
	// Folder operations
	unsigned int list_pid, list_box;			// List a folder
	unsigned int crfl_pid, crfl_box;			// Create a folder
	unsigned int rmfl_pid, rmfl_box;			// Remove a folder
	// General operations
	unsigned int rm_pid, rm_box;				// Remove file/folder
	unsigned int mv_pid, mv_box;				// Move file/folder

	// Linkage
	struct vfs_node_s * next;
	struct vfs_node_s * prev;
	struct vfs_node_s * childs;
	struct vfs_node_s * parent;
} vfs_node_t;

typedef struct{
	vfs_node_t * rootfolder;
	// Device
	unsigned char dev_id[64];
	unsigned int dev_instance;
	unsigned int devdriver_pid;
	unsigned int devdriver_box;
	// Filesystem
	unsigned int fsdriver_pid;
	unsigned int fsdriver_box;
} vfs_mountpoint_t;

typedef struct vfs_fsdriver_s{
	struct vfs_fsdriver_s * next;
	char name[32];
	unsigned int pid;
	unsigned int box;
} vfs_fsdriver_t;

// VFS messages
// ------------

#define SUBMESSAGE_TYPE_REG_FSDRIVER 1
#define SUBMESSAGE_TYPE_MOUNT 2
#define SUBMESSAGE_TYPE_UMOUNT 4

typedef struct{
	char name[32];
	unsigned int pid;
	unsigned int box;
} vfs_submessage_reg_fsdriver_t __attribute__((__packed__));

typedef struct{
	char fsname[32];
	uint8_t device_id[64];
	uint32_t device_instance;
	uint8_t mountpoint;
	unsigned int permissions;
} vfs_submessage_mount_t __attribute__((__packed__));

typedef struct{
	uint32_t type;
	uint32_t size;
	uint8_t content[];
} vfs_submessage_t __attribute__((__packed__));
typedef struct{
	uint8_t amount_messages;
	vfs_submessage_t messages[];
} vfs_message_t __attribute__((__packed__));

int vfs_main(void * p);

#endif
