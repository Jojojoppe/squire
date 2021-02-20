#ifndef __H_FSDRIVER
#define __H_FSDRIVER 1

#include <stdint.h>
#include <stddef.h>

#include <squire_vfs.h>

// Driver specification
// --------------------
typedef struct{
	uint8_t id[64];
	uint32_t flags;
} squire_fsdriver_supported_t __attribute__((__packed__));
typedef struct{
	char name[64];
	uint16_t version_major, version_minor;
	uint32_t simple_box;
	squire_fsdriver_supported_t supported[];
} squire_fsdriver_t __attribute__((__packed));

#define VFS_FSDRIVER_FLAGS_NODEVICE 1			// If filesystem does not need a device
#define VFS_FSDRIVER_FLAGS_NOCACHE 2			// Tree structure of filesystem cannot be cached

/**
 * @brief FSDRIVER return codes
 */
typedef enum FSDRIVER_FUNCTION{
	FSDRIVER_FUNCTION_REGFSDRIVER_R,
	FSDRIVER_FUNCTION_MOUNT,
	FSDRIVER_FUNCTION_MOUNT_R,
	FSDRIVER_FUNCTION_UNMOUNT,
	FSDRIVER_FUNCTION_UNMOUNT_R,
} squire_fsdriver_function_t;

/**
 * @brief FSDRIVER message structure
 */
typedef struct{
	squire_fsdriver_function_t function;
	uint8_t id[64];
	char string0[256], string1[128], string2[128];
	unsigned int uint0, uint1, uint2, uint3, pid, box;
	void * voidp0, * voidp1;
	size_t size0, size1;
} squire_fsdriver_message_t;

#define VFS_FSDRIVER_PID 1
#define VFS_FSDRIVER_BOX 2
// ---------------------------

// Macro functions for driver development
typedef struct{
	squire_fsdriver_t * fsdriver_info;
	void(*function_callback)(unsigned int from, squire_fsdriver_message_t * msg);
} SQUIRE_FSDRIVER_INFO;

#define FSDRIVER(FSDRIVER_INFO) \
	SQUIRE_FSDRIVER_INFO * _fsdriver_info = &FSDRIVER_INFO ; \
	extern int fsdriver_main(int, char**); \
	int main(int argc, char ** argv){ \
		return fsdriver_main(argc, argv); \
	}

int fsdriver_main(int, char**);

#endif
