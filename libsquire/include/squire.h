#ifndef __H_SQUIRE
#define __H_SQUIRE 1

#include <squire_syscall.h>
#include <squire_vfs.h>
#include <squire_rpc.h>

/**
 * @brief Frequency of a jiffie (kernel tick)
 *
 */
#define SQUIRE_HZ 100

/*
 * Squire basic mountpoints
 */
#define SQUIRE_MP_DDM 0
#define SQUIRE_MP_INITRAMFS 1
#define SQUIRE_MP_PROCFS 2

#endif
