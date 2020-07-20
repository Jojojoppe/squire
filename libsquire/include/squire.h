#ifndef __H_SQUIRE
#define __H_SQUIRE 1

#include <stdint.h>
#include <stdlib.h>

// -------------------
// System call numbers
// -------------------
#define SQUIRE_SYSCALL_YIELD		0x00000000
#define SQUIRE_SYSCALL_MMAP			0x00000001
#define SQUIRE_SYSCALL_THREAD		0x00000010
#define SQUIRE_SYSCALL_PROCESS		0x00000011
#define SQUIRE_SYSCALL_SEND			0x00000020
#define SQUIRE_SYSCALL_RECEIVE		0x00000021
#define SQUIRE_SYSCALL_LOG			0x10000000
// -------------------

// ----------------------------------
// Parameter structs for system calls
// ----------------------------------

struct squire_params_mmap_s{
	void *			address;
	size_t			length;
	uint32_t		flags;
};
typedef struct squire_params_mmap_s squire_params_mmap_t;

struct squire_params_thread_s{
	void 			(*entry)(void);
	void *			stack;
	uint32_t		flags;
};
typedef struct squire_params_thread_s squire_params_thread_t;

struct squire_params_process_s{
	void *			elf_start;
	size_t			elf_length;
};
typedef struct squire_params_process_s squire_params_process_t;

# define SQUIRE_SEND_MAX_DATASIZE 0x4000
struct squire_params_send_s{
	uint32_t		pid;
	uint32_t		flags;
	size_t			size;
	char			data[SQUIRE_SEND_MAX_DATASIZE];
};
typedef struct squire_params_sebd_s squire_params_send_t;

# define SQUIRE_RECEIVE_MAX_DATASIZE 0x4000
struct squire_params_receive_s{
	uint32_t		pid;
	uint32_t		flags;
	size_t			size;
	char			data[SQUIRE_RECEIVE_MAX_DATASIZE]
};
typedef struct squire_params_receive_s squire_params_receive_t;

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

extern void * squire_syscall_mmap(void * address, size_t length, uint32_t flags);
extern uint32_t squire_syscall_thread(void (*entry)(void), void * stack, uint32_t flags);
extern uint32_t squire_syscall_process(void * elf_start, size_t elf_length);
extern uint32_t squire_syscall_process();
extern void squire_syscall_log(char * data, size_t length);

#if defined(__cplusplus)
} /* extern "C" */
#endif
// --------------------

#endif
