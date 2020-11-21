#ifndef __H_SQUIRE_VFS
#define __H_SQUIRE_VFS 1

#include <squire_fsdriver.h>
#include <stdint.h>

/**
 * @brief VFS RPC functions
 */
typedef enum VFS_RPC_FUNCTION{
	VFS_RPC_FUNCTION_MOUNT,
	VFS_RPC_FUNCTION_UMOUNT,
	VFS_RPC_FUNCTION_OPEN,
	VFS_RPC_FUNCTION_CLOSE,
	VFS_RPC_FUNCTION_READ,
	VFS_RPC_FUNCTION_WRITE,
	VFS_RPC_FUNCTION_LSDIR,
	VFS_RPC_FUNCTION_MKDIR,
	VFS_RPC_FUNCTION_REMOVE,
	VFS_RPC_FUNCTION_MOVE,
	VFS_RPC_FUNCTION_FSTAT,
	VFS_RPC_FUNCTION_RETURN		// Status message returned by RPC
} squire_vfs_rpc_function_t;

/**
 * @brief VFS RPC return codes
 */
typedef enum VFS_RPC_RETURN{
	VFS_RPC_RETURN_NOERR = 0,
	VFS_RPC_RETURN_ERR,
	VFS_RPC_RETURN_ALREADY_MOUNTED,
	VFS_RPC_RETURN_CANNOT_MOUNT,
	VFS_RPC_RETURN_NOT_MOUNTED,
	VFS_RPC_RETURN_CANNOT_UNMOUNT,
} squire_vfs_rpc_return_t;

/**
 * @brief VFS message structure
 */
typedef struct{
	squire_vfs_rpc_function_t function;
	char string0[256], string1[128], string2[128];
	unsigned int uint0, uint1, uint2, uint3, box;
	void * voidp0, * voidp1;
	size_t size0, size1;
} squrie_vfs_message_t;

#define VFS_PID 1
#define VFS_BOX 1
#define VFS_OUTBOX 255

// File permissions
#define VFS_PERMISSIONS_READALL 1
#define VFS_PERMISSIONS_WRITEALL 2
#define VFS_PERMISSIONS_EXECALL 4
#define VFS_PERMISSIONS_READOWN 8
#define VFS_PERMISSIONS_WRITEOWN 16
#define VFS_PERMISSIONS_EXECOWN 32

squire_vfs_rpc_return_t vfs_mount(int mountpoint, char fsname[32], char device_id[64], unsigned int device_instance, int permissions);
squire_vfs_rpc_return_t vfs_unmount(int mountpoint);

#endif
