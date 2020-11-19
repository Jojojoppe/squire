#ifndef __H_SQUIRE_VFS
#define __H_SQUIRE_VFS 1

#include <squire_fsdriver.h>
#include <stdint.h>

int squire_vfs_mount(uint8_t mountpoint, unsigned int owner, unsigned int permissions, char * fsname, uint8_t device_id[64], uint32_t device_instance);

#endif
