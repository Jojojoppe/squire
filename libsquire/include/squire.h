#ifndef __H_SQUIRE
#define __H_SQUIRE 1

#include <stdlib.h>

// -------------------
// System call numbers
// -------------------
#define SQUIRE_SYSCALL_MMAP			0x00000001
#define SQUIRE_SYSCALL_THREAD		0x00000010
#define SQUIRE_SYSCALL_PROCESS		0x00000011
#define SQUIRE_SYSCALL_JOIN         0x00000012
#define SQUIRE_SYSCALL_EXIT			0x00000013
#define SQUIRE_SYSCALL_SIMPLE_SEND	0x00000020
#define SQUIRE_SYSCALL_SIMPLE_RECV	0x00000021
#define SQUIRE_SYSCALL_LOG			0x10000000
// -------------------

// ----------------------------------
// Parameter structs for system calls
// ----------------------------------

struct squire_params_mmap_s{
	void * 			address;
	size_t			length;
	unsigned int	flags;
};
typedef struct squire_params_mmap_s squire_params_mmap_t;

// ---

struct squire_params_thread_s{
	void 			(*entry)(void);
	void *			stack_base;
	size_t			stack_length;
	unsigned int	flags;
};
typedef struct squire_params_thread_s squire_params_thread_t;

struct squire_params_process_s{
	void *			elf_start;
	size_t			elf_length;
	int 			argc;
	void *			param_data;
	size_t			param_data_size;
	unsigned int	pid;
};
typedef struct squire_params_process_s squire_params_process_t;

struct squire_params_join_s{
	unsigned int	id;
	int				retval;
};
typedef struct squire_params_join_s squire_params_join_t;

struct squire_params_exit_s{
	int				retval;
};
typedef struct squire_params_exit_s squire_params_exit_t;

// ---

struct squire_params_simple_send_s{
	unsigned int 	to;
	void *			data;
	size_t			length;
	unsigned int	status;
};
typedef struct squire_params_simple_send_s squire_params_simple_send_t;

struct squire_params_simple_recv_s{
	unsigned int	from;
	void *			buffer;
	size_t			length;
	unsigned int	status;
	unsigned char	blocked;
};
typedef struct squire_params_simple_recv_s squire_params_simple_recv_t;

// ---

struct squire_params_log_s{
	char *			data;
	size_t			length;
};
typedef struct squire_params_log_s squire_params_log_t;

// ----------------------------------

// --------------------
// System call wrappers
// --------------------
#if defined(__cplusplus)
extern "C" {
#endif

extern void * squire_syscall_mmap(void * address, size_t length, unsigned int flags);

extern unsigned int squire_syscall_thread(void (*entry)(void), void * stack_base, size_t stack_length, unsigned int flags, void * param);
extern unsigned int squire_syscall_process(void * elf_start, size_t elf_length, int argc, char ** argv);
extern int squire_syscall_join(unsigned int id);
extern void squire_syscall_exit(int retval);

extern unsigned int squire_syscall_simple_send(unsigned int to, size_t length, void * data);
extern unsigned int squire_syscall_simple_recv(void * buffer, size_t * length, unsigned int * from);
extern unsigned int squire_syscall_simple_recv_blocked(void * buffer, size_t * length, unsigned int * from);

extern void squire_syscall_log(char * data, size_t length);

#if defined(__cplusplus)
} /* extern "C" */
#endif
// --------------------

#endif
