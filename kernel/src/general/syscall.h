#ifndef __H_SYSCALL
#define __H_SYSCALL 1

#include <general/stdint.h>

// -------------------
// System call numbers
// -------------------
#define SQUIRE_SYSCALL_MMAP			0x00000001
#define SQUIRE_SYSCALL_LOG			0x10000000
// -------------------

// ----------------------------------
// Parameter structs for system calls
// ----------------------------------

struct squire_params_mmap_s{
	void * 			address;
	size_t			length;
	uint32_t		flags;
};
typedef struct squire_params_mmap_s squire_params_mmap_t;

struct squire_params_log_s{
	char *			data;
	size_t			length;
};
typedef struct squire_params_log_s squire_params_log_t;

// ----------------------------------

enum SYSCALL_ERROR{
    SYSCALL_ERROR_GENERAL = -1,
    SYSCALL_ERROR_PARAMS = -2
};

unsigned syscall(unsigned int opcode, void * param_block, size_t param_len);

#endif