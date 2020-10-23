#ifndef __H_SYSCALL
#define __H_SYSCALL 1

#include <general/stdint.h>

// -------------------
// System call numbers
// -------------------
#define SQUIRE_SYSCALL_MMAP			0x00000001
#define SQUIRE_SYSCALL_THREAD		0x00000010
#define SQUIRE_SYSCALL_PROCESS		0x00000011
#define SQUIRE_SYSCALL_JOIN         0x00000012
#define SQUIRE_SYSCALL_EXIT			0x00000013
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

struct squire_params_thread_s{
	void 			(*entry)(void);
	void *			stack_base;
    size_t          stack_length;
	uint32_t		flags;
};
typedef struct squire_params_thread_s squire_params_thread_t;

struct squire_params_process_s{
	void *			elf_start;
	size_t			elf_length;
	int 			argc;
	void *			param_data;
	size_t			param_data_size;
};
typedef struct squire_params_process_s squire_params_process_t;

struct squire_params_join_s{
	unsigned int	id;
    int             retval;
};
typedef struct squire_params_join_s squire_params_join_t;

struct squire_params_exit_s{
	int				retval;
};
typedef struct squire_params_exit_s squire_params_exit_t;

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