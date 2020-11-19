#include <squire_vfs.h>
#include <squire.h>
#include <squire_fsdriver.h>

int squire_vfs_mount(uint8_t mountpoint, unsigned int owner, unsigned int permissions, char * fsname, uint8_t device_id[64], uint32_t device_instance){

	size_t msglen = sizeof(vfs_message_t)+sizeof(vfs_submessage_t)+sizeof(vfs_submessage_mount_t);
	vfs_message_t * msg = (vfs_message_t*)malloc(msglen);
	msg->amount_messages = 1;
	vfs_submessage_t * mntsubmsg = msg->messages;
	mntsubmsg->type = SUBMESSAGE_TYPE_MOUNT;
	mntsubmsg->size = sizeof(vfs_submessage_mount_t);
	vfs_submessage_mount_t * mntmsg = (vfs_submessage_mount_t*)mntsubmsg->content;
	mntmsg->mountpoint = mountpoint;
	mntmsg->owner = owner;
	strcpy(mntmsg->fsname, fsname);
	mntmsg->device_instance = device_instance;
	memcpy(mntmsg->device_id, device_id, 64);
	mntmsg->permissions = permissions;
	squire_message_simple_box_send(msg, msglen, 1, 1);
	free(msg);


	return 0;
}
