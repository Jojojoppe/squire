#ifndef __H_FSDRIVER
#define __H_FSDRIVER

#include <stdint.h>

#define VFS_FSDRIVER_FLAGS_NODEVICE 1			// If filesystem does not need a device
#define VFS_FSDRIVER_FLAGS_NOCACHE 2			// Tree structure of filesystem cannot be cached

// File permissions
#define PERMISSIONS_READALL 1
#define PERMISSIONS_WRITEALL 2
#define PERMISSIONS_EXECALL 4
#define PERMISSIONS_READOWN 8
#define PERMISSIONS_WRITEOWN 16
#define PERMISSIONS_EXECOWN 32

typedef enum VFS_NODE_TYPE{
	VFS_NODE_TYPE_FILE,
	VFS_NODE_TYPE_DIRECTORY,
	VFS_NODE_TYPE_LINK
} vfs_node_type_t;

typedef struct{
	vfs_node_type_t type;
	char name[128];
	unsigned int permissions;
	unsigned int owner;
	void * fsdriver_private;
} vfs_lsdir_node_t;

// VFS messages
// ------------

// Messages to the VFS
#define SUBMESSAGE_TYPE_REG_FSDRIVER 1
#define SUBMESSAGE_TYPE_MOUNT 2
#define SUBMESSAGE_TYPE_UMOUNT 4

#define SUBMESSAGE_TYPE_FSMOUNT 8				// Message to FSDRIVER to mount
#define SUBMESSAGE_TYPE_FSMOUNT_R 16			// Response from FSDRIVER to mount
#define SUBMESSAGE_TYPE_FSLSDIR 32
#define SUBMESSAGE_TYPE_FSLSDIR_R 64

typedef struct{
	char name[32];
	uint8_t flags;
	unsigned int pid;
	unsigned int box;
} vfs_submessage_reg_fsdriver_t __attribute__((__packed__));

typedef struct{
	uint8_t mountpoint;
	char fsname[32];
	unsigned int permissions;
	unsigned int owner;
	// If fsdriver need a device
	uint8_t device_id[64];
	uint32_t device_instance;
} vfs_submessage_mount_t __attribute__((__packed__));

typedef struct{
	uint8_t mountpoint;
	char fsname[32];
	uint8_t device_id[64];
	uint32_t device_instance;
} vfs_submessage_fsmount_t __attribute__((__packed__));

typedef struct{
	uint8_t mountpoint;
	void * fsdriver_private_mount;		// FSdriver specific data for mountpoint
	void * fsdriver_private_root;		// FSdriver specific data for root dir
} vfs_submessage_fsmount_r_t __attribute__((__packed__));

typedef struct{
	void * fsdriver_private_mount;		// FSdriver specific data for mountpoint
	void * fsdriver_private;			// FSdriver specific data for directory
} vfs_submessage_fslsdir_t __attribute__((__packed__));

typedef struct{
	char shared_id[32];
	unsigned int shared_length;
	unsigned int nr_nodes;				// Amount of nodes returned by fslsdir
} vfs_submessage_fslsdir_r_t __attribute__((__packed__));

typedef struct{
	uint32_t type;
	uint32_t size;
	uint8_t content[];
} vfs_submessage_t __attribute__((__packed__));
typedef struct{
	uint8_t amount_messages;
	vfs_submessage_t messages[];
} vfs_message_t __attribute__((__packed__));



extern unsigned int vfs_pid;
extern unsigned int vfs_box;

typedef struct{
	char fsname[64];		// Filesystem type
	unsigned int flags;		// flags VFS_FSDRIVER_*
} squire_fsdriver_supported_t;
typedef struct{
	char name[64];
	uint16_t version_major, version_minor;
	uint32_t simple_box;
	squire_fsdriver_supported_t supported[];
} squire_fsdriver_t;

typedef struct{
	squire_fsdriver_t * fsdriver_info;
	int (*fsmount_callback)(void ** r_private_mount, void ** r_private_root, uint8_t mountpoint, char fsnane[32], uint8_t device_id[64], uint32_t device_instance);
	int (*fslsdir_callback)(int * r_nr_nodes, size_t * r_shared_size, void ** r_shared, void * private_mount, void * private_dir, char shared_id[32]);
} SQUIRE_FSDRIVER_INFO;

#define FSDRIVER(FSDRIVER_INFO) \
	SQUIRE_FSDRIVER_INFO * _fsdriver_info = &FSDRIVER_INFO ; \
	extern int fsdriver_main(int, char**);\
	int main(int argc, char ** argv){ \
		return fsdriver_main(argc, argv); \
	}

#endif
