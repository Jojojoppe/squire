#ifndef __H_FSDRIVER_INITRAMFS
#define __H_FSDRIVER_INITRAMFS 1

#include <stddef.h>
#include <stdint.h>

int initramfs_fsdriver(void * p);

int fsmount_callback(void ** r_private_mount, void ** r_private_root, uint8_t mountpoint, char fsnane[32], uint8_t device_id[64], uint32_t device_instance);
int fslsdir_callback(int * r_nr_nodes, size_t * r_shared_size, void ** r_shared, void * private_mount, void * private_dir, char shared_id[32]);

#endif
