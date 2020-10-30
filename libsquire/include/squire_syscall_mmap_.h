#ifndef _H_SQUIRE_SYSCALL_MMAP_
#define _H_SQUIRE_SYSCALL_MMAP_ 1

#define SQUIRE_SYSCALL_MMAP			0x00000001

struct squire_params_mmap_s{
	void * 			address;
	size_t			length;
	unsigned int	flags;
};
typedef struct squire_params_mmap_s squire_params_mmap_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern void * squire_syscall_mmap(void * address, size_t length, unsigned int flags);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
