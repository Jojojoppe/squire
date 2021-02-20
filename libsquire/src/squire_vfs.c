#include <squire_vfs.h>
#include <squire_rpc.h>
#include <squire.h>

#include <string.h>

// Create VFS_OUTBOX mutex
squire_mutex_t m_vfs_outbox = 0;

squire_vfs_rpc_return_t squire_vfs_mount(int mountpoint, char fsname[32], char device_id[64], unsigned int device_instance, int permissions){

	squire_vfs_message_t msg;
	msg.function = VFS_RPC_FUNCTION_MOUNT;
	msg.uint0 = mountpoint;
	msg.uint1 = device_instance;
	msg.uint2 = permissions;
	strcpy(msg.string0, fsname);
	memcpy(msg.string1, device_id, 64);
	msg.box = VFS_OUTBOX;

	// Wait until OUTBOX is free
	if(m_vfs_outbox==0){
		m_vfs_outbox = squire_mutex_create();
	}
	squire_mutex_lock(m_vfs_outbox);

	size_t msg_len = sizeof(msg);
	int rpc_status = squire_rpc_box(VFS_OUTBOX, VFS_PID, VFS_BOX, &msg, sizeof(msg), &msg, msg_len);

	squire_mutex_unlock(m_vfs_outbox);

	if(rpc_status){
		return VFS_RPC_RETURN_ERR;
	}

	return msg.uint0;
}

squire_vfs_rpc_return_t squire_vfs_unmount(int mountpoint){

	squire_vfs_message_t msg;
	msg.function = VFS_RPC_FUNCTION_UNMOUNT;
	msg.uint0 = mountpoint;
	msg.box = VFS_OUTBOX;

	// Wait until OUTBOX is free
	if(m_vfs_outbox==0){
		m_vfs_outbox = squire_mutex_create();
	}
	squire_mutex_lock(m_vfs_outbox);

	size_t msg_len = sizeof(msg);
	int rpc_status = squire_rpc_box(VFS_OUTBOX, VFS_PID, VFS_BOX, &msg, sizeof(msg), &msg, msg_len);

	squire_mutex_unlock(m_vfs_outbox);

	if(rpc_status){
		return VFS_RPC_RETURN_ERR;
	}

	// TODO check return message msg

	return VFS_RPC_RETURN_NOERR;
}
