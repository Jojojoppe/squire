#ifndef __H_SQUIRE_VFS
#define __H_SQUIRE_VFS 1

#include <stdint.h>
#include <stddef.h>
#include <dirent.h>

/*
 * Driver interface
 */

typedef struct{
	char type[64];
} squire_vfs_driver_supported_t;

typedef struct{
	// Basic driver info
	char name[64];								// Name of driver
	uint16_t version_major, version_minor;		// Version of driver
	unsigned int pid;							// PID of driver, filled in by fsdriver_main
	unsigned int box;							// Public box for communication
	// Function callbacks
	int (*mount)(char * type, char * device, unsigned int mountpoint, unsigned int flags);
	int (*umount)(unsigned int mountpoint);

	int (*opendir)(char * path, struct dirent * dirent);
	int (*readdir)(unsigned int current_entry, struct dirent * dirent);

	int (*open)(char * path, unsigned int * fd);
	int (*close)(unsigned int fd);
	int (*read)(unsigned int fd, size_t offset, size_t * length, char * buf);

	squire_vfs_driver_supported_t supported[32];	

	// Internal stuff
	char rand_key[33];
	char vfs_signature[33]; // Signature of the VFS
	void * openfiles;
} squire_vfs_driver_t;

#define SQUIRE_VFS_DRIVER(DRIVER_INFO) \
	extern squire_vfs_driver_t * __vfs_driver_info = &driver_info; \
	extern int squire_vfs_driver_main(int, char**); \
	int main(int argcm char ** argv){ \
		return squire_vfs_driver_main(argc, argv); \
		thrd_create(0, 0, 0); printf(""); \
	}

/*
 * ------
 */

/*
 * VFS interface
 */

#define SQUIRE_VFS_DRIVER_BOX 20
#define SQUIRE_VFS_USER_BOX 21
#define SQUIRE_VFS_PID 1

// Submessage type								// box: message direction
typedef enum{
	SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER,		// VFS-DRIVER: DRIVER->VFS
	SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER_R,	// VFS-DRIVER: VFS->DRIVER
	SQUIRE_VFS_SUBMESSAGE_MOUNT,				// VSF-DRIVER and USER-VFS: VFS->DRIVER and USER->VFS
	SQUIRE_VFS_SUBMESSAGE_MOUNT_R,				// VSF-DRIVER and USER-VFS: DRIVER->VFS and VFS->USER
	SQUIRE_VFS_SUBMESSAGE_OPENDIR,				// VFS-DRIVER and USER-VFS: VFS-DRIVER and USER->VFS
	SQUIRE_VFS_SUBMESSAGE_OPENDIR_R,			// DRIVER-USER: DRIVER->USER
	SQUIRE_VFS_SUBMESSAGE_READDIR,
	SQUIRE_VFS_SUBMESSAGE_READDIR_R,
	SQUIRE_VFS_SUBMESSAGE_OPEN,
	SQUIRE_VFS_SUBMESSAGE_OPEN_R,
	SQUIRE_VFS_SUBMESSAGE_CLOSE,
	SQUIRE_VFS_SUBMESSAGE_CLOSE_R,
	SQUIRE_VFS_SUBMESSAGE_READ,
	SQUIRE_VFS_SUBMESSAGE_READ_R,
} squire_vfs_submessage_type_t;

// Main message header
typedef struct{
	uint32_t messages;
	uint32_t length;
	char signature[32+1];
} squire_vfs_message_header_t;

typedef struct{
	squire_vfs_submessage_type_t submessage_type;
	uint32_t length;
} squire_vfs_submessage_header_t;

/*
 * Content of submessages
 * ----------------------
 * 
 * + SQUIRE_VFS_SUBMESSAGE_REGISTER_DRIVER
 * 		contains directly after the header a copy of the driver_info structure
 * + SQUIRE_VFS_SUBMESSAGE_MOUNT_(R)
 * 		contains squire_vfs_submessage_mount_t
 * + SQUIRE_VFS_SUBMESSAGE_OPENDIR_(R)
 * 		contains squire_vfs_submessage_dir_t structure
 */

typedef struct{
	char device[64];
	char type[64];
	unsigned int mountpoint;
	unsigned int pid, box;
	int status;
	char signature[33];	// Signature of calling user process
} squire_vfs_submessage_mount_t;

typedef struct{
	unsigned int mountpoint;
	unsigned int pid, box;
	unsigned int dpid, dbox;
	int status;
	char path[MAXNAMLEN+1];
	struct dirent dirent;
	char signature[33];	// Signature of calling user process
} squire_vfs_submessage_dir_t;

typedef struct{
	unsigned int mountpoint;
	unsigned int pid, box;
	unsigned int dpid, dbox;
	int status;
	char path[MAXNAMLEN+1];
	int fdesc;
	size_t length;
	size_t offset;
	uint64_t nonce;
	char signature[33];	// Signature of calling user process
} squire_vfs_submessage_file_t;


#define VFS_MSG_LEN(x) (sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_message_header_t) + x)
void * squire_vfs_create_message(void ** msg, size_t length, char * signature, int type);

int squire_vfs_driver_main_direct(int argc, char ** argv, squire_vfs_driver_t * driver_info);
int squire_vfs_user_mount(char * type, char * device, unsigned int mountpoint);

#endif
