#ifndef __H_SQUIRE_VFS
#define __H_SQUIRE_VFS 1

#include <stdint.h>
#include <stddef.h>

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
	void (*umount)(unsigned int mountpoint);
	squire_vfs_driver_supported_t supported[32];	
} squire_vfs_driver_t;

#define SQUIRE_VFS_DRIVER(DRIVER_INFO) \
	squire_vfs_driver_t * __vfs_driver_info = &driver_info; \
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
	SQUIRE_VFS_SUBMESSAGE_MOUNT,				// VSF-DRIVER and USER-VFS: VFS->DRIVER and USER->VFS
	SQUIRE_VFS_SUBMESSAGE_MOUNT_R,				// VSF-DRIVER and USER-VFS: DRIVER->VFS and VFS->USER
} squire_vfs_submessage_type_t;

// Main message header
typedef struct{
	uint32_t messages;
	uint32_t length;
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
 */

typedef struct{
	char device[64];
	char type[64];
	unsigned int mountpoint;
	unsigned int pid, box;
	int status;
} squire_vfs_submessage_mount_t;

int squire_vfs_user_mount(char * type, char * device, unsigned int mountpoint);

#endif
